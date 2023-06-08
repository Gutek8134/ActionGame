// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameStatics.h"

const UStaticItemData* UActionGameStatics::GetItemStaticData(TSubclassOf<UStaticItemData> ItemDataClass) {
	if (IsValid(ItemDataClass))
		return GetDefault<UStaticItemData>(ItemDataClass);
	
	return nullptr;
}