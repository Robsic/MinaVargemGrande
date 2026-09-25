// Copyright (c) 2021 Laboratory of Robotics, Intelligent and 
// Complex Systems (RobSIC) at the Universidade Federal
// de Itajubá (UNIFEI), Campus of Itabira.
//
// This work is licensed under the terms of XXX license.


#pragma once

//#include "Vehicle/VehicleControl.h"

#include "PanelControlCAT793F.generated.h"




USTRUCT(BlueprintType)
struct SIRV_API FCAT793FPanelControl //: FPanelControl
{
  GENERATED_BODY()
  
//---CAT793F----------------------------------------------------------------------------------------
  
  	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
  	bool seatbelt 		= false;       
  	
  	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)       
	bool firesupression 	= false;        
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool engineidle 	= false;            
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool tractioncontrol 	= false;        
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool parkingbrakeretract = false;   
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool motion_sw_load 	= false;      
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool motion_sw_unload 	= false;     
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool retardercontrol = false; //(0 - Down, 1 - Up)  
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool autoretarder    = false; //(0 - Down, 1 - Up)       
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool throttlelock    = false;        
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool window          = false; //(0 - Down, 1 - Up)
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool incrementgear   = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool decrementgear   = false;    
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool foglight        = false;   
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool hazerdlight     = false;  
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool headlightlow    = false;  
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite) 
	bool enginekeyon     = false;  
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite) 
	bool enginekeycrank  = false;     
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool stairwaylight   = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool headlighthigh   = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool heatedmirror    = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool dimmer  	     = false;//(0 - Down, 1 - Up)
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool leftturnsignal  = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool rightturnsignal = false;

	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool wiperlow  	     = false;	
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool wiperhigh       = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool windshieldwasherfluid = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool highbean        = false;
	
	UPROPERTY(Category = "Panel Control", EditAnywhere, BlueprintReadWrite)
	bool horn            = false;
  
};
