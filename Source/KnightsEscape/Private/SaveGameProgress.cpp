// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameProgress.h"

USaveGameProgress::USaveGameProgress()
{
	SaveName = TEXT("Saved Game");
	UserIndex = 0;

	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");
}