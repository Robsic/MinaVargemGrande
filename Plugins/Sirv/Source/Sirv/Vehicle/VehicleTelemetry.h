// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once


#include "VehicleTelemetry.generated.h"




USTRUCT(BlueprintType)
struct SIRV_API FVehicleTelemetry
{
  GENERATED_BODY()

	FVehicleTelemetry() = default;
	
  	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)
  	float speed 		= 0.0;       
  	
  	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)       
	float enginerpm 	= 0.0;        
	
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)       
	int gear 	= 0;        

	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)
	float    tryposition 	= 0.0;            
	
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)       
	float payload 		= 0.0;     
	
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)       
	float torque 		= 0.0;  
	
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)       
	float rollingresistance	= 0.0;  
		
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)
	FVector position = {0.0f, 0.0f, 0.0f};   

	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)
	FVector orientation = {0.0f, 0.0f, 0.0f};    
	
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)
	FVector acceleration = {0.0f, 0.0f, 0.0f}; 
	
	UPROPERTY(Category = "Vehicle Telemetry", EditAnywhere, BlueprintReadWrite)
	FVector angularvelocity = {0.0f, 0.0f, 0.0f}; 
	
};
