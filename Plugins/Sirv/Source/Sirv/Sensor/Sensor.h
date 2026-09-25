// Copyright (c) 2023 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.

#pragma once

#include "Game/SirvEpisode.h"

#include "Ros/SirvRosBinder.h"
//#include "Sirv/Sensor/DataStream.h"

#include "sirv/Buffer.h"

#include "Util/RandomEngine.h"

#include "GameFramework/Actor.h"

#include "Sensor.generated.h"

struct FActorDescription;

/// Base class for sensors.
UCLASS(Abstract, hidecategories = (Collision, Attachment, Actor))
class SIRV_API ASensor : public AActor
{
  GENERATED_BODY()

public:

  ASensor(const FObjectInitializer &ObjectInitializer);

  void SetEpisode(const USirvEpisode &InEpisode)
  {
    Episode = &InEpisode;
  }

  void SetRosNode(const USirvRosBinder &InRosNode)
  {
    Ros = &InRosNode;
  }

  void send_data()
  {
    int a = 3;
  }


  virtual void Set(const FActorDescription &Description);

  virtual void BeginPlay();

  /// Replace the FDataStream associated with this sensor.
  ///
  /// @warning Do not change the stream after BeginPlay. It is not thread-safe.
  //void SetDataStream(FDataStream InStream)
  //{
  //  Stream = std::move(InStream);
  //}

  /// Return the token that allows subscribing to this sensor's stream.
  //auto GetToken() const
  //{
  //  return Stream.GetToken();
  //}

  void Tick(const float DeltaTime) final;

  virtual void PrePhysTick(float DeltaSeconds) {}
  virtual void PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds) {}

  UFUNCTION(BlueprintCallable)
  URandomEngine *GetRandomEngine()
  {
    return RandomEngine;
  }

  UFUNCTION(BlueprintCallable)
  int32 GetSeed() const
  {
    return Seed;
  }

  UFUNCTION(BlueprintCallable)
  void SetSeed(int32 InSeed);


protected:

  void PostActorCreated() override;

  void EndPlay(EEndPlayReason::Type EndPlayReason) override;

  const USirvEpisode &GetEpisode() const
  {
    check(Episode != nullptr);
    return *Episode;
  }

  /// Return the FDataStream associated with this sensor.
  ///
  /// You need to provide a reference to self, this is necessary for template
  /// deduction.
  //template <typename SensorT>
  //FAsyncDataStream GetDataStream(const SensorT &Self)
  //{
  //  return Stream.MakeAsyncDataStream(Self, GetEpisode().GetElapsedGameTime());
  //}

  const USirvRosBinder *GetRos() const
  {
    check(Ros != nullptr);
    return Ros;
  }

  

  /// Seed of the pseudo-random engine.
  UPROPERTY(Category = "Random Engine", EditAnywhere)
  int32 Seed = 123456789;

  /// Random Engine used to provide noise for sensor output.
  UPROPERTY()
  URandomEngine *RandomEngine = nullptr;


  sirv::Buffer buf;
  int32 topic_id=0;
  bool is_publishing=false;

private:

  

  FDelegateHandle OnPostTickDelegate;

  const USirvEpisode *Episode = nullptr;
  
  //FDataStream Stream;
  
  const USirvRosBinder *Ros = nullptr;

 

  


};
