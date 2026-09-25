#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Texture2D.h"
#include "Interfaces/IHttpRequest.h"
#include "ElevationSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeightmapReceived, UTexture2D*, LoadedHeightmap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpticalImageLoaded, UTexture2D*, LoadedOpticalTexture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCalibrationReceived, float, ScaleFactor, float, OffsetFactor);

UCLASS(BlueprintType)
class SATILLITE_IMAGERY_API UElevationSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(BlueprintAssignable, Category = "Geospatial|Events")
    FOnHeightmapReceived OnHeightmapReceived;

    UPROPERTY(BlueprintAssignable, Category = "Geospatial|Events")
    FOnOpticalImageLoaded OnOpticalImageLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Geospatial|Events")
    FOnCalibrationReceived OnCalibrationReceived;

    UFUNCTION(BlueprintCallable, Category = "Geospatial|IO")
    UTexture2D* LoadOpticalImageFromFile(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "Geospatial|HTTP")
    void RequestCalibration(const FString& SatelliteImagePath, const FString& ReferenceDEMPath);

    UFUNCTION(BlueprintCallable, Category = "Elevation")
    void RequestRelativeDepth(const FString& ImageFilePath);

private:
    void HandleDepthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};