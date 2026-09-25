#include "ElevationSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "ImageUtils.h"
#include "Engine/Texture2D.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UElevationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("ElevationSubsystem Initialized."));
}

void UElevationSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UElevationSubsystem::RequestRelativeDepth(const FString& ImageFilePath)
{
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *ImageFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load image at: %s"), *ImageFilePath);
        return;
    }

    FString Boundary = TEXT("----UE5Boundary") + FString::FromInt(FMath::Rand());
    FString Filename = FPaths::GetCleanFilename(ImageFilePath);

    FString Header = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: application/octet-stream\r\n\r\n"), *Boundary, *Filename);
    FString Footer = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);

    TArray<uint8> Payload;
    FTCHARToUTF8 ConverterHeader(*Header);
    Payload.Append((uint8*)ConverterHeader.Get(), ConverterHeader.Length());
    Payload.Append(FileData);
    FTCHARToUTF8 ConverterFooter(*Footer);
    Payload.Append((uint8*)ConverterFooter.Get(), ConverterFooter.Length());

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://127.0.0.1:8000/infer/relative"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));
    Request->SetContent(Payload);

    Request->OnProcessRequestComplete().BindUObject(this, &UElevationSubsystem::HandleDepthResponse);
    Request->ProcessRequest();

    UE_LOG(LogTemp, Log, TEXT("Sent depth inference request for %s"), *Filename);
}

void UElevationSubsystem::HandleDepthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
    if (!bConnectedSuccessfully || !Response.IsValid() || Response->GetResponseCode() != 200)
    {
        int32 Code = Response.IsValid() ? Response->GetResponseCode() : -1;
        UE_LOG(LogTemp, Error, TEXT("Inference request failed. HTTP Code: %d"), Code);
        return;
    }

    const TArray<uint8>& RawData = Response->GetContent();
    if (RawData.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Received empty heightmap payload."));
        return;
    }

    UTexture2D* HeightmapTexture = FImageUtils::ImportBufferAsTexture2D(RawData);
    if (HeightmapTexture)
    {
        HeightmapTexture->SRGB = false;
        HeightmapTexture->CompressionSettings = TC_EditorIcon;
        HeightmapTexture->UpdateResource();

        OnHeightmapReceived.Broadcast(HeightmapTexture);
        UE_LOG(LogTemp, Log, TEXT("Heightmap texture created and broadcast successfully (%dx%d)."), 
            HeightmapTexture->GetSizeX(), HeightmapTexture->GetSizeY());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FImageUtils failed to decode returned heightmap image buffer."));
    }
}

UTexture2D* UElevationSubsystem::LoadOpticalImageFromFile(const FString& FilePath)
{
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        return nullptr;
    }

    UTexture2D* OpticalTexture = FImageUtils::ImportBufferAsTexture2D(FileData);
    if (!OpticalTexture)
    {
        return nullptr;
    }

    OpticalTexture->SRGB = true;
    OpticalTexture->UpdateResource();

    OnOpticalImageLoaded.Broadcast(OpticalTexture);
    return OpticalTexture;
}

void UElevationSubsystem::RequestCalibration(const FString& SatelliteImagePath, const FString& ReferenceDEMPath)
{
    TArray<uint8> SatelliteData;
    TArray<uint8> DemData;

    if (!FFileHelper::LoadFileToArray(SatelliteData, *SatelliteImagePath) ||
        !FFileHelper::LoadFileToArray(DemData, *ReferenceDEMPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read files for calibration."));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://127.0.0.1:8000/infer/calibrate"));
    Request->SetVerb(TEXT("POST"));

    const FString Boundary = FString::Printf(TEXT("---------------------------boundary%08x"), FPlatformTime::Cycles());
    Request->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));

    TArray<uint8> Payload;
    auto AppendField = [&Payload, &Boundary](const FString& FieldName, const FString& FileName, const TArray<uint8>& Data)
    {
        FString Header = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: application/octet-stream\r\n\r\n"), *Boundary, *FieldName, *FileName);
        FTCHARToUTF8 Conv(*Header);
        Payload.Append((uint8*)Conv.Get(), Conv.Length());
        Payload.Append(Data);
        FTCHARToUTF8 Eol(TEXT("\r\n"));
        Payload.Append((uint8*)Eol.Get(), Eol.Length());
    };

    AppendField(TEXT("optical_image"), FPaths::GetCleanFilename(SatelliteImagePath), SatelliteData);
    AppendField(TEXT("reference_dem"), FPaths::GetCleanFilename(ReferenceDEMPath), DemData);

    FString Footer = FString::Printf(TEXT("--%s--\r\n"), *Boundary);
    FTCHARToUTF8 ConvFooter(*Footer);
    Payload.Append((uint8*)ConvFooter.Get(), ConvFooter.Length());

    Request->SetContent(Payload);

    Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bConnectedSuccessfully)
    {
        if (!bConnectedSuccessfully || !Res.IsValid() || Res->GetResponseCode() != 200)
        {
            UE_LOG(LogTemp, Error, TEXT("Calibration request failed."));
            return;
        }

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const float Scale = JsonObject->GetNumberField(TEXT("scale"));
            const float Offset = JsonObject->GetNumberField(TEXT("offset"));

            const float ScaleInUU = Scale * 100.0f;
            const float OffsetInUU = Offset * 100.0f;

            OnCalibrationReceived.Broadcast(ScaleInUU, OffsetInUU);
        }
    });

    Request->ProcessRequest();
}