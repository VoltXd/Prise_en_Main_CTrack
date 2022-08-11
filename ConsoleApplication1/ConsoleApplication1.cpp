#include "pch.h"
#include <iostream>
#include <iomanip>
#include <msclr/auto_gcroot.h>
#include <Windows.h>
#include "ConsoleApplication1.h"

using namespace System;
using namespace VXelementsApi;

//Console Handler
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


int main(array<System::String ^> ^args)
{
	try
	{
		ApiManager::Connect();
	}
	catch (VXelementsApi::VXelementsException ^ e)
	{
		std::cout << "Erreur :" << std::endl;
		std::cout << "\tImpossible d'ouvrir VXelements.\n\n";
		exit(EXIT_FAILURE);
	}
	
	//Déclarations des variables de VXelements
	msclr::auto_gcroot<IVXelements^> mVXelements;
	msclr::auto_gcroot<VXtrack::IVXtrack^> mVXtrack;
	msclr::auto_gcroot<VXtrack::ITrackingSequence^> pSequence;
	
	mVXelements = ApiManager::VXelements;
	mVXtrack = mVXelements->VXtrack;
	float sampleRate = mVXtrack->TrackingParameters->GetSamplingRate();

	// Sur VXelements : 
	// Scénario 1
	// 1. Track -> affiche les cibles
	//
	// Scénario 2
	// 1. Détection de Modèle (Imposition du modèle...)
	// 2. Ajout d'une séquence 
	// 3. Start

	//Scénario 1 (le plus "fiable")
	mVXtrack->DetectModel();
	std::cout << "Waiting for a tracking model!" << std::endl;

	//Scénario 2 (le plus rapide)
	//mVXtrack->ImportModel("ModeleZephyr.txt");
	
	while (mVXtrack->TrackingEntities->Length == 0)
		Threading::Thread::Sleep(1);
	mVXtrack->ShowModelsStatusForm();		//Console
	mVXtrack->ShowProjectionViewForm();		//Oscilloscope
	cli::array<VXtrack::ITrackingEntity^>^ entities = mVXtrack->TrackingEntities;
	mVXtrack->StartTracking();
	pSequence = mVXtrack->CurrentTrackingSequence;

	while (ApiManager::IsConnected)
	{
		if (entities->Length > 0)
		{
			VXtrack::ITrackingEntity^ zephyrEntity = entities[0];
			cli::array<Types::IPositioningTargets^>^ targets;
			
			std::cout << "Position des Cibles : " << std::endl;
			
			targets = pSequence->GetLastPositioningTargets(zephyrEntity);
			int targetCount;

			if (targets == nullptr)
				std::cout << "Targets nullptr" << std::endl;
			else
			{
				targetCount = targets->Length;
				for (int i = 0; i < targetCount; i++)
				{
					Types::IPositioningTargets^ target = targets[i];
					double xPos = target->Center.X;
					double yPos = target->Center.Y;
					double zPos = target->Center.Z;

					printColor("Target Index : ", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					printColor(i, BACKGROUND_RED | BACKGROUND_INTENSITY);
					std::cout << ", ";
					printColor("X : ", FOREGROUND_RED | FOREGROUND_INTENSITY);
					printColor(xPos, BACKGROUND_RED | BACKGROUND_INTENSITY);
					std::cout << ", ";
					printColor("Y : ", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					printColor(yPos, BACKGROUND_RED | BACKGROUND_INTENSITY);
					std::cout << ", ";
					printColor("Z : ", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					printColor(zPos, BACKGROUND_RED | BACKGROUND_INTENSITY);
					std::cout << std::endl;
				}
			}

			std::cout << std::endl << "Position Zephyr : " << std::endl;
			Types::IPose3d^ pPose;
			pPose = pSequence->GetLastPose(zephyrEntity);
			if (pPose == nullptr)
				std::cout << "X : NULL, Y : NULL, Z : NULL" << std::endl;
			else
			{
				double xPos = pPose->Translation.X;
				double yPos = pPose->Translation.Y;
				double zPos = pPose->Translation.Z;

				printColor("Target Count : ", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				printColor(targetCount, BACKGROUND_RED | BACKGROUND_INTENSITY);
				std::cout << ", ";
				printColor("X : ", FOREGROUND_RED | FOREGROUND_INTENSITY);
				printColor(xPos, BACKGROUND_RED | BACKGROUND_INTENSITY);
				std::cout << ", ";
				printColor("Y : ", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				printColor(yPos, BACKGROUND_RED | BACKGROUND_INTENSITY);
				std::cout << ", ";
				printColor("Z : ", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				printColor(zPos, BACKGROUND_RED | BACKGROUND_INTENSITY);
				std::cout << std::endl;
			}
		}
		else
			std::cout << "pas d'entites" << "\r";
		Threading::Thread::Sleep(TimeSpan::FromMilliseconds(1e3f / sampleRate));
		system("cls");
	}

	std::cout << "Hello world!" << std::endl;
    return 0;
}

template <typename T> void printColor(const T text, int color)
{
	SetConsoleTextAttribute(hConsole, color);
	std::cout << text;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}
