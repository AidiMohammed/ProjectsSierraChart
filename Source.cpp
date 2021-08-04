#include <WinSock2.h>
#include "sierrachart.h"
#include "scstudyfunctions.h"

void InitWinSock();

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

SCDLLName("Sav Data 30 Level V132")

SCSFExport scsf_SenkouSpan(SCStudyInterfaceRef sc)
{
	SCInputRef Input_StudySubgraphReference = sc.Input[0];
	SCInputRef Input_StudySubgraphReferenceB = sc.Input[1];
	SCInputRef Input_StudySubgraphReferanceExp = sc.Input[2];
	SCInputRef Input_StudySubgraphReferanceWeig = sc.Input[3];

	SCSubgraphRef FlacheAchat = sc.Subgraph[0];
	SCSubgraphRef FlacheVente = sc.Subgraph[1];
	SCSubgraphRef FlacheTarget = sc.Subgraph[2];
	SCSubgraphRef FlacheStop = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Mohammed Aidi Pour Rachid VER 3.5";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_AUTO;

		int DisplayOrder = 1;

		FlacheAchat.Name = "Signial Achat";
		FlacheAchat.PrimaryColor = RGB(0, 0, 200);
		FlacheAchat.DrawStyle = DRAWSTYLE_ARROWUP;
		FlacheAchat.LineWidth = 2;

		FlacheVente.Name = "Signial Vent";
		FlacheVente.PrimaryColor = RGB(200, 0, 0);
		FlacheVente.DrawStyle = DRAWSTYLE_ARROWDOWN;
		FlacheVente.LineWidth = 2;

		FlacheTarget.Name = "Signial Target";
		FlacheTarget.PrimaryColor = RGB(0, 200, 0);
		FlacheTarget.DrawStyle = DRAWSTYLE_ARROWLEFT;
		FlacheTarget.LineWidth = 2;

		FlacheStop.Name = "Signial Stop";
		FlacheStop.PrimaryColor = RGB(230, 20, 0);
		FlacheStop.DrawStyle = DRAWSTYLE_ARROWRIGHT;
		FlacheStop.LineWidth = 2;

		Input_StudySubgraphReference.Name = "Ligne (SPAN A)";
		Input_StudySubgraphReference.SetStudySubgraphValues(0, 0);

		Input_StudySubgraphReferenceB.Name = "Ligne (SPAN B)";
		Input_StudySubgraphReferenceB.SetStudySubgraphValues(0, 0);

		Input_StudySubgraphReferanceExp.Name = "Moving Average Exponential";
		Input_StudySubgraphReferanceExp.SetStudySubgraphValues(0, 0);

		Input_StudySubgraphReferanceWeig.Name = "Moving Average Weighted";
		Input_StudySubgraphReferanceWeig.SetStudySubgraphValues(0, 0);

		return;
	}


	SCFloatArrayRef Last = sc.Close;

	if (sc.IsFullRecalculation)
		return;

	int BarTime = sc.BaseDateTimeIn[sc.Index].GetTime();
	bool TradingAllowedSession = BarTime >= HMS_TIME(15, 40, 0) && BarTime < HMS_TIME(19, 0, 0);

	int& r_ActivationTradingAuto = sc.GetPersistentInt(1);

	int Result = 0;

	// Ceci renverra sc.UpdateStartIndex ou un index antérieur si cette étude est calculée après une étude calculée à un index précédent et prenant en charge la définition de cet index.
	int ActualStartIndex = sc.GetCalculationStartIndexForStudy();

	SCFloatArray Study1Array,
				 Study1ArrayB,
				 StudyArrayExp,
				 StudyArrayWeig;

	sc.GetStudyArrayUsingID(Input_StudySubgraphReference.GetStudyID(), Input_StudySubgraphReference.GetSubgraphIndex(), Study1Array);
	sc.GetStudyArrayUsingID(Input_StudySubgraphReferenceB.GetStudyID(), Input_StudySubgraphReferenceB.GetSubgraphIndex(), Study1ArrayB);
	sc.GetStudyArrayUsingID(Input_StudySubgraphReferanceExp.GetStudyID(), Input_StudySubgraphReferanceExp.GetSubgraphIndex(), StudyArrayExp);
	sc.GetStudyArrayUsingID(Input_StudySubgraphReferanceWeig.GetStudyID(), Input_StudySubgraphReferanceWeig.GetSubgraphIndex(), StudyArrayWeig);

	float LigneA = (float)Study1Array[sc.Index - 26];
	float LigneB = (float)Study1ArrayB[sc.Index - 26];
	float Expo = (float)StudyArrayExp[sc.Index];
	float Weig = (float)StudyArrayWeig[sc.Index];

	if (TradingAllowedSession)
	{
		//quand le prix se réintègre dans le nuage
		if (Last.ElementAt(sc.Index) < LigneA && Last.ElementAt(sc.Index) > LigneB || Last.ElementAt(sc.Index) < LigneB && Last.ElementAt(sc.Index) > LigneA)
			r_ActivationTradingAuto = 1;

		else
		{
			//Pour Les Achat
			if (sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED &&
				Last.ElementAt(sc.Index) > LigneA &&
				Last.ElementAt(sc.Index) > LigneB &&
				r_ActivationTradingAuto == 1 &&
				Last.ElementAt(sc.Index) > Expo &&
				Last.ElementAt(sc.Index) > Weig)

			{

				sc.PlaySoundW(5);
			}
			//Pour Les Vent
			if (sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED &&
				Last.ElementAt(sc.Index) < LigneA &&
				Last.ElementAt(sc.Index) < LigneB &&
				r_ActivationTradingAuto == 1 &&
				Last.ElementAt(sc.Index) < Expo &&
				Last.ElementAt(sc.Index) < Weig)
			{
				sc.PlaySoundW(5);
			}
		}
		SCString Info = NULL;
		//Info.AppendFormat("Posistion Data = %ld | REF Trading Auto = %ld", (int)PositionData.PositionQuantity, r_ActivationTradingAuto);
		//sc.AddMessageToLog(Info, 1);

		sc.EarliestUpdateSubgraphDataArrayIndex = ActualStartIndex;

	}
}
SCSFExport scsf_UseToolExampleChangeFontBackColor(SCStudyInterfaceRef sc)
{
	// Change font back color
	SCSubgraphRef MoyenneTailleBougies = sc.Subgraph[0];
	SCSubgraphRef NombreDeTickDeBougies = sc.Subgraph[1];
	SCSubgraphRef MovingAvrg = sc.Subgraph[2];

	SCInputRef Length = sc.Input[0];

	// Set configuration variables
	if (sc.SetDefaults)
	{
		sc.GraphName = "calculer la moyenne de la taille des bougies";
		sc.GraphRegion = 2;
		sc.AutoLoop = 0;//pour recalculer tout le graphe

		MoyenneTailleBougies.Name = "Taille de bugies";
		MoyenneTailleBougies.PrimaryColor = RGB(255, 0, 35);
		MoyenneTailleBougies.DrawStyle = DRAWSTYLE_LINE;

		NombreDeTickDeBougies.Name = "Nombre de tick de bougeis";
		NombreDeTickDeBougies.PrimaryColor = RGB(0, 55, 125);
		NombreDeTickDeBougies.DrawStyle = DRAWSTYLE_LINE;

		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		Length.SetDescription("The number of bars to average.");

		sc.AutoLoop = 1; //No automatic looping

		return;
	}
	int& AcumulationDesTickDesBougies = sc.GetPersistentInt(1);

	if (sc.Index > Length.GetInt()-1)
	{
		for (int i = Length.GetInt()-1; i > -1 ; i--)
		{
			AcumulationDesTickDesBougies += sc.BaseData[SC_HIGH][sc.Index-i] - sc.BaseData[SC_LOW][sc.Index-i];
			SCString info;
			info.AppendFormat("| I = %ld | High = %f", i,sc.BaseData[SC_HIGH][sc.Index-i]);
			sc.AddMessageToLog(info, 1);
		}
		MoyenneTailleBougies[sc.Index] = (float)(AcumulationDesTickDesBougies / sc.TickSize) / Length.GetInt()-1;
		NombreDeTickDeBougies[sc.Index] = (float)(sc.BaseData[SC_HIGH][sc.Index] - sc.BaseData[SC_LOW][sc.Index]) / sc.TickSize;
	}

	sc.SetPersistentInt(1, 0);
	SCString Info;
	Info.AppendFormat("Finched | sc.Arrysize - 1 = %ld |sc.Index = %ld", sc.ArraySize - 1,sc.Index);
	sc.AddMessageToLog(Info, 1);
}
SCSFExport scsf_MohammedSavData(SCStudyInterfaceRef sc)
{
	SCInputRef NameFaile = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Sav Data 30 Level";

		sc.StudyDescription = "Sav data 30 Level Ask & Bid (StackPullBidLevel[x],QuantBidLevel[x],PriceBidLevel[x],StackPullAskLevel[x],QuantAskLevel[x],PriceAskLevel[x],Heur,Min,Sec,MillSec";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;
		sc.FreeDLL = 1;
		sc.UsesMarketDepthData = 1;

		NameFaile.Name = "Sympol Name";
		NameFaile.SetString("");


		return;
	}
	SCString Info, Sympol;
	FILE* Fichier = NULL;

	s_MarketDepthEntry MarketDepthBid, MarketDepthAsk;

	sc.GetBidMarketDepthEntryAtLevel(MarketDepthBid, 0);
	sc.GetAskMarketDepthEntryAtLevel(MarketDepthAsk, 0);

	/*int StackPullBidValue = (float)sc.GetBidMarketDepthStackPullValueAtPrice(MarketDepthBid.Price);
	int StackPullAskValue = (float)sc.GetAskMarketDepthStackPullValueAtPrice(MarketDepthAsk.Price);*/



	int StackPullBidValue[30];
	int StackPullAskValue[30];
	int Heur, Min, Sec, MillSec;
	int TotalStask = 0;
	int QuantityAsk[30];
	int QuantityBid[30];

	int& TotalStackLast = sc.GetPersistentInt(1);

	float PriceBid[30];
	float PriceAsk[30];


	Sympol.Append(NameFaile.GetString());

	for (int I = 0; I < 30; I++)
	{
		sc.GetBidMarketDepthEntryAtLevel(MarketDepthBid, I);
		sc.GetAskMarketDepthEntryAtLevel(MarketDepthAsk, I);

		StackPullBidValue[I] = sc.GetBidMarketDepthStackPullValueAtPrice(MarketDepthBid.Price);
		StackPullAskValue[I] = sc.GetAskMarketDepthStackPullValueAtPrice(MarketDepthAsk.Price);

		QuantityBid[I] = (int)MarketDepthBid.Quantity;
		QuantityAsk[I] = (int)MarketDepthAsk.Quantity;

		PriceBid[I] = MarketDepthBid.Price;
		PriceAsk[I] = MarketDepthAsk.Price;
	}

	Fichier = fopen(Sympol + ".DAT", "a+");
	if (Fichier == NULL)
	{
		Info.Clear();
		Info.Append("Impsible d'ouvrier le ficher");

		sc.AddMessageToLog(Info, 0);
	}
	else
	{

		for (int I = 0; I < 30; I++)
			fprintf(Fichier, "%ld %ld %.2f %ld %ld %.2f ", StackPullBidValue[I],QuantityBid[I], PriceBid[I], StackPullAskValue[I],QuantityAsk[I], PriceAsk[I]);

		Heur = sc.BaseDateTimeIn[sc.Index].GetHour();
		Min = sc.BaseDateTimeIn[sc.Index].GetMinute();
		Sec = sc.BaseDateTimeIn[sc.Index].GetSecond();
		MillSec = sc.BaseDateTimeIn[sc.Index].GetMillisecond();

		fprintf(Fichier, "%ld %ld %ld %ld", Heur, Min, Sec, MillSec);

		fprintf(Fichier, "\n");
		fclose(Fichier);

		TotalStackLast = TotalStask;
	}

}
SCSFExport scsf_AfficherData(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskPrice = sc.Subgraph[37];
	SCSubgraphRef BidPrice = sc.Subgraph[38];

	SCInputRef NamFile = sc.Input[0];
	SCInputRef NombreDeLevel = sc.Input[1];
	SCInputRef TopHighOrHighAskEndBid = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Afficher Les Data 30 level VSB";

		sc.StudyDescription = "Afficher les data Enregistrer par le study Sav Data";
		sc.GraphRegion = 0;
		sc.AutoLoop = 1;
		sc.FreeDLL = 1;

		AskPrice.Name = "Ask Price";
		AskPrice.PrimaryColor = RGB(200, 0, 15);
		AskPrice.LineWidth = 5;
		AskPrice.DrawStyle = DRAWSTYLE_DASH;

		BidPrice.Name = "Bid Price";
		BidPrice.PrimaryColor = RGB(15, 0, 200);
		BidPrice.LineWidth = 5;
		BidPrice.DrawStyle = DRAWSTYLE_DASH;

		NamFile.Name = "Name File";
		NamFile.SetString("");

		NombreDeLevel.Name = "Nombre de Level";
		NombreDeLevel.SetInt(10);

		TopHighOrHighAskEndBid.Name = "Top High Or Ask & Bid";
		TopHighOrHighAskEndBid.SetCustomInputStrings("Top High;TopAsk/Bid");
		TopHighOrHighAskEndBid.SetCustomInputIndex(0);

		int IColor = 0;

		for (int I = 0; I < 36; I++)
		{
			if (I < 18)
			{
				sc.Subgraph[I].Name = "ASK COLOR";
				sc.Subgraph[I].PrimaryColor = RGB(255, I * 15, I * 15);
				sc.Subgraph[I].DrawStyle = DRAWSTYLE_COLOR_BACKGROUND_AT_PRICE;
			}
			else
			{
				sc.Subgraph[I].Name = "BID COLOR";
				sc.Subgraph[I].PrimaryColor = RGB(IColor * 15, IColor * 15, 255);
				sc.Subgraph[I].DrawStyle = DRAWSTYLE_COLOR_BACKGROUND_AT_PRICE;
				IColor++;
			}
		}

		return;
	}
	SCString Info = NULL;
	SCString FileName = NamFile.GetString();
	FILE* Fille = NULL;
	int StackPullBidValue[30];
	int StackPullAskValue[30];
	int SommeAskDelta = 0;
	int SommeBidDelta = 0;
	int HighAsk = 0;
	int HighBid = 0;
	int TopHigh = 0;

	int Heur = 0;
	int Min = 0;
	int Sec = 0;
	int MillSec = 0;
	int QASK[30];
	int QBID[30];

	int& Index = sc.GetPersistentInt(1);

	int* LastDataASK = (int*)sc.GetPersistentPointer(1);
	int* LastDataBID = (int*)sc.GetPersistentPointer(2);

	if (sc.LastCallToFunction)
	{
		if (LastDataASK != NULL || LastDataBID != NULL)
		{
			if (LastDataASK != NULL)
			{
				sc.FreeMemory(LastDataASK);
				sc.SetPersistentPointer(1, NULL);
			}

			if (LastDataBID != NULL)
			{
				sc.FreeMemory(LastDataBID);
				sc.SetPersistentPointer(2, NULL);
			}
		}
		return;
	}
	if (LastDataASK == NULL || LastDataBID == NULL)
	{
		if (LastDataASK == NULL)
		{
			LastDataASK = (int*)sc.AllocateMemory(30 * sizeof(int));
			if (LastDataASK != NULL)
				sc.SetPersistentPointer(1, NULL);
			else
				return;
		}
		if (LastDataBID == NULL)
		{
			LastDataBID = (int*)sc.AllocateMemory(30 * sizeof(int));
			if (LastDataBID != NULL)
				sc.SetPersistentPointer(2, NULL);
			else
				return;
		}
	}

	float PriceBid[30];
	float PriceAsk[30];

	int DeltaAsk[30];
	int DeltaBid[30];

	float& LastPriceAsk = sc.GetPersistentFloat(1);
	float& LastPriceBid = sc.GetPersistentFloat(2);

	Fille = fopen(FileName + ".DAT", "r");

	for (; Index != sc.ArraySize - 1; Index++)
	{
		if (Fille == NULL)
		{
			Info.Append("Impsible d'ouvrier le ficher");
			sc.AddMessageToLog(Info, 0);
		}
		else
		{
			for (int I = 0; I < 30; I++)
			{
				//StackPullBidValue[I], QuantityBid[I], PriceBid[I], StackPullAskValue[I], QuantityAsk[I], PriceAsk[I]
				fscanf(Fille, "%ld %ld %f %ld %ld %f ", &StackPullBidValue[I],&QBID[I], &PriceBid[I], &StackPullAskValue[I],&QASK[I], &PriceAsk[I]);

				/*DeltaAsk[I] = StackPullAskValue[I] - LastDataASK[I];
				DeltaBid[I] = StackPullBidValue[I] - LastDataBID[I];*/

				DeltaAsk[I] = StackPullAskValue[I] - LastDataASK[I];
				DeltaBid[I] = StackPullBidValue[I] - LastDataBID[I];

				if (StackPullBidValue[I] > HighBid)HighBid = StackPullBidValue[I];
				if (StackPullAskValue[I] > HighAsk)HighAsk = StackPullAskValue[I];

				LastDataASK[I] = StackPullAskValue[I];
				LastDataBID[I] = StackPullBidValue[I];

			}

			fscanf(Fille, "%ld %ld %ld %ld", &Heur, &Min, &Sec, &MillSec);
		}

		if (HighBid > HighAsk)TopHigh = HighBid;
		else TopHigh = HighAsk;

		for (int I = 0; I <30; I++)
		{
			if (I <= 30 - 1)
			{
				float CofficientMultiplicatuerHighBid;
				float CofficientMultiplicatuerHighAsk;
				if (I == 0)
				{
					BidPrice[Index] = PriceBid[I];
					AskPrice[Index] = PriceAsk[I];
				}
				/*if (StackPullBidValue[I] == HighBid)
					sc.Subgraph[18][Index] = PriceBid[I];
				if (StackPullAskValue[I] == HighAsk)
					sc.Subgraph[0][Index] = PriceAsk[I];*/

				if (TopHighOrHighAskEndBid.GetIndex() == 0)
				{
					CofficientMultiplicatuerHighBid = (float)TopHigh / 100;
					CofficientMultiplicatuerHighAsk = (float)TopHigh / 100;
				}
				else if (TopHighOrHighAskEndBid.GetIndex() == 1)
				{
					CofficientMultiplicatuerHighBid = (float)HighBid / 100;
					CofficientMultiplicatuerHighAsk = (float)HighAsk / 100;
				}
				float pourcentatagIntBid = StackPullBidValue[I] / CofficientMultiplicatuerHighBid;
				float pourcentatagIntAsk = StackPullAskValue[I] / CofficientMultiplicatuerHighAsk;

				Info.Clear();
				Info.AppendFormat("(TopHig = %ld)  SPullBid= %ld / CofiBid= %f | SpullAsk= %ld / CofiAsk %f | pourcentatagIntAsk = %f | pourcentatagIntBid = %f ",TopHigh,StackPullBidValue[I],CofficientMultiplicatuerHighBid,StackPullAskValue[I] , CofficientMultiplicatuerHighAsk, pourcentatagIntAsk, pourcentatagIntBid);
				sc.AddMessageToLog(Info, 0);

				if (pourcentatagIntBid >= 0 && pourcentatagIntBid <= 5.5 || pourcentatagIntAsk >= 0 && pourcentatagIntAsk <= 5.5)
				{
					if (pourcentatagIntBid >= 0 && pourcentatagIntBid <= 5.5)
						sc.Subgraph[35][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 0 && pourcentatagIntAsk <= 5.5)
						sc.Subgraph[17][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 5.5 && pourcentatagIntBid <= 11 || pourcentatagIntAsk >= 5.5 && pourcentatagIntAsk <= 11)
				{
					if (pourcentatagIntBid >= 5.5 && pourcentatagIntBid <= 11)
						sc.Subgraph[34][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 5.5 && pourcentatagIntAsk <= 11)
						sc.Subgraph[16][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 11 && pourcentatagIntBid <= 16.5 || pourcentatagIntAsk >= 11 && pourcentatagIntAsk <= 16.5)
				{
					if (pourcentatagIntBid >= 11 && pourcentatagIntBid <= 16.5)
						sc.Subgraph[33][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 11 && pourcentatagIntAsk <= 16.5)
						sc.Subgraph[15][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 16.5 && pourcentatagIntBid <= 22 || pourcentatagIntAsk >= 16.5 && pourcentatagIntAsk <= 22)
				{
					if (pourcentatagIntBid >= 16.5 && pourcentatagIntBid <= 22)
						sc.Subgraph[32][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 16.5 && pourcentatagIntAsk <= 22)
						sc.Subgraph[14][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 22 && pourcentatagIntBid <= 27.5 || pourcentatagIntAsk >= 22 && pourcentatagIntAsk <= 27.5)
				{
					if (pourcentatagIntBid >= 22 && pourcentatagIntBid <= 27.5)
						sc.Subgraph[31][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 22 && pourcentatagIntAsk <= 27.5)
						sc.Subgraph[13][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 27.5 && pourcentatagIntBid <= 33 || pourcentatagIntAsk >= 27.5 && pourcentatagIntAsk <= 33)
				{
					if (pourcentatagIntBid >= 27.5 && pourcentatagIntBid <= 33)
						sc.Subgraph[30][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 27.5 && pourcentatagIntAsk <= 33)
						sc.Subgraph[12][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 33 && pourcentatagIntBid <= 38.5 || pourcentatagIntAsk >= 33 && pourcentatagIntAsk <= 38.5)
				{
					if (pourcentatagIntBid >= 33 && pourcentatagIntBid <= 38.5)
						sc.Subgraph[29][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 33 && pourcentatagIntAsk <= 38.5)
						sc.Subgraph[11][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 38.5 && pourcentatagIntBid <= 44 || pourcentatagIntAsk >= 38.5 && pourcentatagIntAsk <= 44)
				{
					if (pourcentatagIntBid >= 38.5 && pourcentatagIntBid <= 44)
						sc.Subgraph[28][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 38.5 && pourcentatagIntAsk <= 44)
						sc.Subgraph[10][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 44 && pourcentatagIntBid <= 49.5 || pourcentatagIntAsk >= 44 && pourcentatagIntAsk <= 49.5)
				{
					if (pourcentatagIntBid >= 44 && pourcentatagIntBid <= 49.5)
						sc.Subgraph[27][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 44 && pourcentatagIntAsk <= 49.5)
						sc.Subgraph[9][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 49.5 && pourcentatagIntBid <= 55 || pourcentatagIntAsk >= 49.5 && pourcentatagIntAsk <= 55)
				{
					if (pourcentatagIntBid >= 49.5 && pourcentatagIntBid <= 55)
						sc.Subgraph[26][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 49.5 && pourcentatagIntAsk <= 55)
						sc.Subgraph[8][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 55 && pourcentatagIntBid <= 60.5 || pourcentatagIntAsk >= 55 && pourcentatagIntAsk <= 60.5)
				{
					if (pourcentatagIntBid >= 55 && pourcentatagIntBid <= 60.5)
						sc.Subgraph[25][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 55 && pourcentatagIntAsk <= 60.5)
						sc.Subgraph[7][Index] = PriceAsk[I];
				} 
				if (pourcentatagIntBid >= 60.5 && pourcentatagIntBid <= 66 || pourcentatagIntAsk >= 60.5 && pourcentatagIntAsk <= 66)
				{
					if (pourcentatagIntBid >= 60.5 && pourcentatagIntBid <= 66)
						sc.Subgraph[24][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 60.5 && pourcentatagIntAsk <= 66)
						sc.Subgraph[6][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 66 && pourcentatagIntBid <= 71.5 || pourcentatagIntAsk >= 66 && pourcentatagIntAsk <= 71.5)
				{
					if (pourcentatagIntBid >= 66 && pourcentatagIntBid <= 71.5)
						sc.Subgraph[23][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 66 && pourcentatagIntAsk <= 71.5)
						sc.Subgraph[5][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 71.5 && pourcentatagIntBid <= 77 || pourcentatagIntAsk >= 71.5 && pourcentatagIntAsk <= 77)
				{
					if (pourcentatagIntBid >= 71.5 && pourcentatagIntBid <= 77)
						sc.Subgraph[22][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 71.5 && pourcentatagIntAsk <= 77)
						sc.Subgraph[4][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 77 && pourcentatagIntBid <= 82.5 || pourcentatagIntAsk >= 77 && pourcentatagIntAsk <= 82.5)
				{
					if (pourcentatagIntBid >= 77 && pourcentatagIntBid <= 82.5)
						sc.Subgraph[21][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 77 && pourcentatagIntAsk <= 82.5)
						sc.Subgraph[3][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 82.5 && pourcentatagIntBid <= 88 || pourcentatagIntAsk >= 82.5 && pourcentatagIntAsk <= 88)
				{
					if (pourcentatagIntBid >= 82.5 && pourcentatagIntBid <= 88)
						sc.Subgraph[20][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 82.5 && pourcentatagIntAsk <= 88)
						sc.Subgraph[2][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 88 && pourcentatagIntBid <= 93.5 || pourcentatagIntAsk >= 88 && pourcentatagIntAsk <= 93.5)
				{
					if (pourcentatagIntBid >= 88 && pourcentatagIntBid <= 93.5)
						sc.Subgraph[19][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 88 && pourcentatagIntAsk <= 93.5)
						sc.Subgraph[1][Index] = PriceAsk[I];
				}
				if (pourcentatagIntBid >= 93.5 && pourcentatagIntBid <= 100 || pourcentatagIntAsk >= 93.5 && pourcentatagIntAsk <= 100)
				{
					if (pourcentatagIntBid >= 93.5 && pourcentatagIntBid <= 100)
						sc.Subgraph[18][Index] = PriceBid[I];
					if (pourcentatagIntAsk >= 93.5 && pourcentatagIntAsk <= 100)
						sc.Subgraph[0][Index] = PriceAsk[I];
				}

			}

		}

		HighBid = 0;
		HighAsk = 0;
		TopHigh = 0;
		
	}
	fclose(Fille);
}
SCSFExport scsf_SoketTest(SCStudyInterfaceRef sc)
{
	// Set configuration variables
	SCInputRef portNomber = sc.Input[0];
	SCInputRef depthSize = sc.Input[1];

	SCSubgraphRef Value = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.AutoLoop = 0; 
		sc.MaintainTradeStatisticsAndTradesData = 1;

		portNomber.Name = "Num de port";
		portNomber.SetInt(2404);
		portNomber.SetIntLimits(0, 65535);

		depthSize.Name = "Nombre De lavel Depth (BID&ASK) 0 = All Depth Market";
		depthSize.SetInt(0);
		depthSize.SetIntLimits(0, sc.GetBidMarketDepthNumberOfLevels());

		Value.Name = "Value x";
		Value.PrimaryColor = RGB(200, 0, 0);
		Value.DrawStyle = DRAWSTYLE_LINE;
		Value.LineWidth = 3;
		Value.DrawZeros = true;

		return;
	}

	int& itIsLaunched = sc.GetPersistentInt(1);
	int& SocketPer = sc.GetPersistentInt(2);

	int_fast64_t& lastPreocessedSequence = sc.GetPersistentInt64(1);

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);

	#pragma region Lancer La DSS

		if (PositionData.PositionQuantity != 0 && itIsLaunched == SC_NO)
		{
			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			if (!CreateProcess(TEXT("SAD Trading.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				SCString Info;
				Info.AppendFormat("CreateProcess failed (%ld)", GetLastError());
				return;
			}
			else
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				itIsLaunched = SC_YES;
			}
		}

	#pragma endregion

	#pragma region Send Data si La Dss est lancer

		else if (itIsLaunched == SC_YES)// Si la Dss et Lancer Envoyer les Data
		{
			SCString symbol;
			SCString account;
			SCString PNL = 0;
			float courrentPrice = 0;
			float PricePosition = 0;
			int dBid = 0;
			int dAsk = 0;
			int& volumeAsk = sc.GetPersistentInt(3);
			int& volumeBid = sc.GetPersistentInt(4);
			int sizePosition = 0;
			int SizeDepthAsk = 0;
			int SizeDepthBid = 0;
			int VolumeAcumuler = 0;
			bool positionIsClosed = false;

			if (PositionData.PositionQuantity == 0 && positionIsClosed == false)
			{
				positionIsClosed = true;

				symbol.Clear();
				account.Clear();
				PNL.Clear();
				courrentPrice = 0.0;
				sizePosition = 0;
				dBid = 0;
				dAsk = 0;
				sc.SetPersistentInt64(1, 0);
				closesocket(SocketPer);
				WSACleanup();

				sc.SetPersistentInt(1, 0);
				sc.SetPersistentInt(2, 0);
				sc.SetPersistentInt(3, 0);
				sc.SetPersistentInt(4, 0);
			}
			else
			{
				SCString informationToSend;

				if (SocketPer == 0)
				{
					SOCKET socketClient;
					SOCKADDR_IN c_sin;

					InitWinSock();

					socketClient = socket(AF_INET, SOCK_STREAM, 0);
					c_sin.sin_addr.s_addr = inet_addr("127.0.0.1");// local host;
					c_sin.sin_family = AF_INET;
					c_sin.sin_port = htons(portNomber.GetInt());

					connect(socketClient, (SOCKADDR*)&c_sin, sizeof(c_sin));
					SocketPer = socketClient;
				}

				ProfitLossDisplayFormatEnum Flag = PPLD_CURRENCY_VALUE;

				symbol.Clear();
				account.Clear();
				PNL.Clear();

				symbol.Append(sc.Symbol);
				account.Append(sc.SelectedTradeAccount);
				sc.CreateProfitLossDisplayString(PositionData.OpenProfitLoss, PositionData.PositionQuantity, Flag, PNL);
				courrentPrice = sc.BaseData[SC_LAST][sc.Index];
				PricePosition = PositionData.AveragePrice;
				sizePosition = PositionData.PositionQuantity;

				if (depthSize.GetInt() == 0)
				{
					SizeDepthBid = sc.GetBidMarketDepthNumberOfLevels();
					SizeDepthAsk = sc.GetAskMarketDepthNumberOfLevels();
				}
				else
				{
					SizeDepthAsk = depthSize.GetInt();
					SizeDepthBid = depthSize.GetInt();
				}

				for (int level = 0; level < SizeDepthBid; level++)
				{
					s_MarketDepthEntry DepthEntryBid;
					sc.GetBidMarketDepthEntryAtLevel(DepthEntryBid, level);

					dBid += DepthEntryBid.Quantity;
				}
				for (int level = 0; level < SizeDepthAsk; level++)
				{
					s_MarketDepthEntry DepthEntryAsk;
					sc.GetAskMarketDepthEntryAtLevel(DepthEntryAsk, level);

					dAsk += DepthEntryAsk.Quantity;
				}

				if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
					lastPreocessedSequence = 0;

				c_SCTimeAndSalesArray TimeSales;
				sc.GetTimeAndSales(TimeSales);

				if (TimeSales.Size() == 0)
					return;

				if (lastPreocessedSequence == 0)
					lastPreocessedSequence = TimeSales[TimeSales.Size() - 1].Sequence;

				int volumeAskOfthisSequence = 0;
				int volumeBidOfthisSequence = 0;

				for (int TSIndex = 0; TSIndex < TimeSales.Size(); TSIndex++)
				{

					if (TimeSales[TSIndex].Sequence <= lastPreocessedSequence)
						continue;

					if (TimeSales[TSIndex].Type == SC_TS_BID)
					{
						volumeBidOfthisSequence += TimeSales[TSIndex].Volume;
						volumeAskOfthisSequence = 0;
					}
					if (TimeSales[TSIndex].Type == SC_TS_ASK)
					{
						volumeAskOfthisSequence += TimeSales[TSIndex].Volume;
						volumeBidOfthisSequence = 0;
					}
					SCDateTime RecrdAdjustedDatetime = TimeSales[TSIndex].DateTime;
					RecrdAdjustedDatetime += sc.TimeScaleAdjustment;
				}

				volumeAsk += volumeAskOfthisSequence;
				volumeBid += volumeBidOfthisSequence;

				Value[sc.Index] = volumeBid + volumeAsk;

				informationToSend.AppendFormat("%s;%s;%s;%.2f;%.2f;%ld;%ld;%ld;%ld;%ld;%ld", symbol.GetChars(), account.GetChars(), PNL.GetChars(), PricePosition, courrentPrice, dAsk, dBid, sizePosition, positionIsClosed, volumeBid, volumeAsk);
				char* informationLength;

				informationLength = (char*)malloc(informationToSend.GetLength() * sizeof(char));

				for (int i = 0; i < informationToSend.GetLength(); i++)
					informationLength[i] = informationToSend[i];

				send(SocketPer, (char*)informationLength, informationToSend.GetLength(), 0);

				free(informationLength);
			}
		}

	#pragma endregion
}
SCSFExport scsf_TimeAndSalesIterationExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskVolume = sc.Subgraph[0];
	SCSubgraphRef BidVolume = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Time and Sales Iteration Example";

		sc.StudyDescription = "This is an example of iterating through the time and sales records which have been added since the last study function call.";

		sc.AutoLoop = 0;//Use manual looping

		AskVolume.Name = "Ask Volume";
		AskVolume.DrawStyle = DRAWSTYLE_LINE;
		AskVolume.PrimaryColor = RGB(255, 0, 0);
		AskVolume.LineWidth = 2;

		BidVolume.Name = "Bid Volume";
		BidVolume.DrawStyle = DRAWSTYLE_LINE;
		BidVolume.PrimaryColor = RGB(0, 0, 255);
		BidVolume.LineWidth = 2;

		sc.GraphRegion = 0;

		return;
	}
	int64_t& LastProssesedSequnce = sc.GetPersistentInt64(1);
	int& p_acumulationBid = sc.GetPersistentInt(1);
	int& p_acumulationAsk = sc.GetPersistentInt(2);
	int& p_LastTSIndex = sc.GetPersistentInt(3);

	if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
		LastProssesedSequnce = 0;

	c_SCTimeAndSalesArray TimeSales;
	sc.GetTimeAndSales(TimeSales);

	if (TimeSales.Size() == 0)
		return;
	if (LastProssesedSequnce == 0)
		LastProssesedSequnce = TimeSales[TimeSales.Size()].Sequence;

	for (int TSIndex = TimeSales.Size(); TSIndex >= 0; TSIndex--)
	{
		if (TimeSales[TSIndex].Sequence <= LastProssesedSequnce)
			continue;
		 
		s_TimeAndSales TS = TimeSales[TSIndex];

		if (TimeSales[TSIndex].Type == SC_TS_BID)
		{
			p_acumulationBid += TimeSales[TSIndex].Volume;
			SCString Info;
			Info.AppendFormat("p_LastTSIndex = %ld | LastProssesedSequnce = %ld | TimeSales[TSIndex].Secance = %ld | p_acumulationBid = %ld | MicroSecond = %ld", (int)p_LastTSIndex, (int)LastProssesedSequnce, (int)TimeSales[TSIndex].Sequence, (int)p_acumulationBid,(int)sc.BaseDataEndDateTime[sc.Index].GetMillisecondsSinceBaseDate());
			sc.AddMessageToLog(Info, 1);
		}
		else if (TimeSales[TSIndex].Type == SC_TS_ASK)
		{
			p_acumulationAsk += TimeSales[TSIndex].Volume;
			SCString Info;
			Info.AppendFormat("p_LastTSIndex = %ld | LastProssesedSequnce = %ld | TimeSales[TSIndex].Secance = %ld | p_acumulationAsk = %ld | | MicroSecond = %ld",(int)p_LastTSIndex, (int)LastProssesedSequnce, (int)TimeSales[TSIndex].Sequence, (int)p_acumulationAsk,(int)sc.BaseDataEndDateTime[sc.Index].GetMillisecondsSinceBaseDate());
			sc.AddMessageToLog(Info, 1);
		}
		SCDateTime RecoardAdjusedDateTime = TimeSales[TSIndex].DateTime;
		RecoardAdjusedDateTime += sc.TimeScaleAdjustment;
	}
	sc.AddMessageToLog("---------------------------------------------------------- ", 1);
	
	p_LastTSIndex = TimeSales.Size();


	AskVolume[sc.Index] = p_acumulationAsk;
	BidVolume[sc.Index] = p_acumulationBid;
	LastProssesedSequnce = TimeSales.Size();
}

SCSFExport scsf_PercentChangeDay(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_PercentChange = sc.Subgraph[0];

	SCInputRef Input_StartTime = sc.Input[1];
	SCInputRef Input_EndTime = sc.Input[2];

	SCString Debug;

	if (sc.SetDefaults)
	{
		sc.GraphName = "Percent Change Day";
		sc.AutoLoop = 0;
		sc.GraphRegion = 1;

		Subgraph_PercentChange.Name = "Percent Change";
		Subgraph_PercentChange.PrimaryColor = RGB(0, 255, 0);
		Subgraph_PercentChange.DrawStyle = DRAWSTYLE_STAIR_STEP;
		Subgraph_PercentChange.LineWidth = 1;

		Input_StartTime.Name = "Start Time : (HH:MM:SS)";
		Input_StartTime.SetTime(0);

		Input_EndTime.Name = "End Time : (HH:MM:SS)";
		Input_EndTime.SetTime(SECONDS_PER_DAY - 1);

		return;
	}

	SCDateTime InStartTime = Input_StartTime.GetDateTime();
	SCDateTime InEndTime = Input_EndTime.GetDateTime();

	int& StartRange = sc.GetPersistentInt(1);
	int& EndRange = sc.GetPersistentInt(2);
	int& FoundPriceForTheBeforeDay = sc.GetPersistentInt(3);

	//Price The day before
	float& PriceCloseTheDayBefore = sc.GetPersistentFloat(1);

	if (sc.LastCallToFunction)
	{
		sc.SetPersistentInt(1, 0);
		sc.SetPersistentInt(2, 0);
		sc.SetPersistentInt(3, 0);

		sc.SetPersistentFloat(1, 0);

		return;
	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		float PriceCloseLastIndex = sc.BaseData[SC_LAST][Index - 1];
		bool StartIsGreaterTheEndOfTheTimeSlot;
		float LastPrice = sc.BaseData[SC_LAST][Index];

		SCDateTime DaysToCalculateStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.ArraySize - 1]) - (INT_MAX - 1);

		if (sc.BaseDateTimeIn[Index] < DaysToCalculateStartDateTime)
			continue;

		SCDateTime StartDateTime = 0;
		SCDateTime EndDateTime = 0;


		SCDateTime BarDate = sc.BaseDateTimeIn[Index].GetDate();
		if (InStartTime < InEndTime)
		{
			StartDateTime = BarDate + InStartTime;
			EndDateTime = BarDate + InEndTime;
		}
		//Les heures sont invesées et l'heure acutelle et supérieure ou égale a l'heure de début
		else if (sc.BaseDateTimeIn[Index].GetTimeInSeconds() >= InStartTime.GetTimeInSeconds())
		{

			StartDateTime = BarDate + InStartTime;
			EndDateTime = BarDate + 1 + InEndTime;
		}
		//Les heures sont inversées et l'heure actuelle est inférieure ou égale à l'heure de fin
		else if (sc.BaseDateTimeIn[Index].GetTimeInSeconds() <= InEndTime.GetTimeInSeconds())
		{

			StartDateTime = BarDate + InStartTime - 1;
			EndDateTime = BarDate + InEndTime;
		}

		if (sc.BaseDateTimeIn[Index] > EndDateTime || sc.BaseDateTimeIn[Index] < StartDateTime)
		{

			if (StartRange == 1)
			{
				StartRange = 0;
				PriceCloseTheDayBefore = PriceCloseLastIndex;
			}

			continue;
		}

		if (StartRange == 0 && Index != 0)
			StartRange = 1;
		
		float PercentChange = ((LastPrice - PriceCloseTheDayBefore) / PriceCloseTheDayBefore) * 100;

		Subgraph_PercentChange[Index] = PercentChange;
	}
}

SCSFExport scsf_Study_Dss_Trading2405(SCStudyInterfaceRef sc)
{
	// Set configuration variables 
	SCInputRef portNomber = sc.Input[0];
	SCInputRef depthSize = sc.Input[1];
	SCInputRef PullingStacking = sc.Input[2];
	SCInputRef Symbol = sc.Input[3];

	//SCSubgraphRef Value = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		sc.MaintainTradeStatisticsAndTradesData = 1;
		sc.UsesMarketDepthData = 1;
		sc.GraphName = "Study For DSS V 2.02 beta";

		portNomber.Name = "Num de port";
		portNomber.SetInt(2404);
		portNomber.SetIntLimits(0, 65535);

		depthSize.Name = "Nombre de levels depths (BID&ASK) 0 = All Levles)";
		depthSize.SetInt(0);

		PullingStacking.Name = "Nombre de levels Puling/Stcking 0 = All Levels";
		PullingStacking.SetInt(4);

		Symbol.Name = "Second Symbol";
		Symbol.SetSymbol("AAA");
		
		return;
	}
	//il faut pas oubler d'ajouter un test pour verifer les valuer entré par l'utilisateur "depthSize" est "PullingStacking"

	int& itIsLaunched = sc.GetPersistentInt(1);
	int& SocketPer = sc.GetPersistentInt(2);
	int& volumeAsk = sc.GetPersistentInt(3);
	int& volumeBid = sc.GetPersistentInt(4);
	int& VolumeBidSecndSymbol = sc.GetPersistentInt(5);
	int& VolumeAskSecndSympol = sc.GetPersistentInt(6);

	if (sc.LastCallToFunction)
	{
		closesocket(SocketPer);
		WSACleanup();
	}

	SCString informationToSend;

	if (sc.Index == 0)return;

	SCString Debug;

	int_fast64_t& LastProssesedSequnce = sc.GetPersistentInt64(1);
	int_fast64_t& LastProssesedSequnce2 = sc.GetPersistentInt64(2);

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);

	#pragma region Lancer La DSS

	if (itIsLaunched == SC_NO)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(TEXT("DSS Trading.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			SCString Info;
			Info.AppendFormat("CreateProcess failed (%ld)", GetLastError());
			return;
		}
		else
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			itIsLaunched = SC_YES;
		}
	}

	#pragma endregion

	#pragma region Send Data si La Dss est lancer

	else if (itIsLaunched == SC_YES)// Si la Dss et Lancer Envoyer les Data
	{
		SCString symbol;
		SCString account;
		SCString PNL = 0;
		float courrentPrice = 0;
		float PricePosition = 0;

		int dBid = 0;
		int dAsk = 0;
		int pullingStachkingAcumuleBid = 0;
		int pullingStachkingAcumuleAsk = 0;

		int_fast64_t& p_LastSequance = sc.GetPersistentInt64(3);
		int_fast64_t& p_LastSequance2 = sc.GetPersistentInt64(4);
		
		int sizePosition = 0;
		int SizeDepthAsk = 0;
		int SizeDepthBid = 0;
		int SizePulingStackingBid = 0;
		int SizePulingStackingAsk = 0;
		bool positionIsClosed = false;

		if (PositionData.PositionQuantity == 0 && positionIsClosed == false)
		{
			positionIsClosed = true;
			symbol.Clear();
			account.Clear();
			PNL.Clear();
			sizePosition = 0;
		}
		if (SocketPer == 0)
		{
			SOCKET socketClient;
			SOCKADDR_IN c_sin;

			InitWinSock();

			socketClient = socket(AF_INET, SOCK_STREAM, 0);
			c_sin.sin_addr.s_addr = inet_addr("127.0.0.1");// local host;
			c_sin.sin_family = AF_INET;
			c_sin.sin_port = htons(portNomber.GetInt());

			connect(socketClient, (SOCKADDR*)&c_sin, sizeof(c_sin));
			SocketPer = (int)socketClient;
		}

		ProfitLossDisplayFormatEnum Flag = PPLD_CURRENCY_VALUE;

		symbol.Clear();
		account.Clear();
		PNL.Clear();

		

		symbol.Append(sc.Symbol);
		account.Append(sc.SelectedTradeAccount);
		sc.CreateProfitLossDisplayString(PositionData.OpenProfitLoss, PositionData.PositionQuantity, Flag, PNL);
		courrentPrice = sc.BaseData[SC_LAST][sc.Index];
		PricePosition = (float)PositionData.AveragePrice;
		sizePosition = (int)PositionData.PositionQuantity;

		#pragma region GetSizeDepthAskAndBid

			if (depthSize.GetInt() == 0)
			{
				SizeDepthBid = sc.GetBidMarketDepthNumberOfLevels();
				SizeDepthAsk = sc.GetAskMarketDepthNumberOfLevels();
			}
			else if (depthSize.GetInt() != 0)
			{

				SizeDepthAsk = depthSize.GetInt();
				SizeDepthBid = depthSize.GetInt();
			}

		#pragma endregion

		#pragma region GetSizePullingStakingBidAsk

			if (PullingStacking.GetInt() == 0)
			{
				SizePulingStackingBid = sc.GetBidMarketDepthNumberOfLevels();
				SizePulingStackingAsk = sc.GetAskMarketDepthNumberOfLevels();
			}
			else if (PullingStacking.GetInt() != 0)
			{
				SizePulingStackingBid = PullingStacking.GetInt();
				SizePulingStackingAsk = PullingStacking.GetInt();
			}

		#pragma endregion

		#pragma region GetDepthAskAndBidAcumulation

			for (int level = 0; level < SizeDepthBid; level++)
			{
				s_MarketDepthEntry DepthEntryBid;
				sc.GetBidMarketDepthEntryAtLevel(DepthEntryBid, level);

				dBid += (int)DepthEntryBid.Quantity;
			}
			for (int level = 0; level < SizeDepthAsk; level++)
			{
				s_MarketDepthEntry DepthEntryAsk;
				sc.GetAskMarketDepthEntryAtLevel(DepthEntryAsk, level);

				dAsk += (int)DepthEntryAsk.Quantity;
			}

		#pragma endregion 

		#pragma region GetPullingStackingAcumulation

			for (int level = 0; level < SizePulingStackingBid; level++)
			{
				s_MarketDepthEntry MarketDepth;
				sc.GetBidMarketDepthEntryAtLevel(MarketDepth, level);

				pullingStachkingAcumuleBid += (int)sc.GetBidMarketDepthStackPullValueAtPrice(MarketDepth.Price);
			}
			for (int level = 0; level < SizePulingStackingAsk; level++)
			{
				s_MarketDepthEntry MarketDepth;
				sc.GetAskMarketDepthEntryAtLevel(MarketDepth, level);

				pullingStachkingAcumuleAsk += (int)sc.GetAskMarketDepthStackPullValueAtPrice(MarketDepth.Price);
			}

		#pragma endregion

		#pragma region GetVolumeTrading

			if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
				LastProssesedSequnce = 0;

			c_SCTimeAndSalesArray TimeSales;
			//sc.GetTimeAndSalesForSymbol(Symbol.GetSymbol(),TimeSales);
			sc.GetTimeAndSales(TimeSales);

			if (TimeSales.Size() == 0)
				return;
			if (LastProssesedSequnce == 0)
			{
				LastProssesedSequnce = TimeSales[TimeSales.Size() - 1].Sequence;
				p_LastSequance = LastProssesedSequnce;
			}
			int sequanc = 0;
			for (int TSIndex = 0; TSIndex <= TimeSales.Size(); TSIndex++)
			{
				if (TimeSales[TSIndex].Sequence <= LastProssesedSequnce)
					continue;

				p_LastSequance = TimeSales[TSIndex].Sequence;
				if (TimeSales[TSIndex].Type == SC_TS_BID)
					volumeBid += TimeSales[TSIndex].Volume;

				if (TimeSales[TSIndex].Type == SC_TS_ASK)
					volumeAsk += TimeSales[TSIndex].Volume;

				
			}

			if (TimeSales.Size() == TimeSales.MaxSize())
				LastProssesedSequnce = p_LastSequance;
			else
				LastProssesedSequnce = TimeSales.GetArraySize();

		#pragma endregion

		#pragma region GetVolumeTradingSegandSymbole

			if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
				LastProssesedSequnce2 = 0;

			c_SCTimeAndSalesArray TimeSales2;
			
			sc.GetTimeAndSalesForSymbol(Symbol.GetSymbol(),TimeSales2);

			if (TimeSales2.Size() != 0)
			{
				if (LastProssesedSequnce2 == 0)
				{
					LastProssesedSequnce2 = TimeSales2[TimeSales2.Size() - 1].Sequence;
					p_LastSequance2 = LastProssesedSequnce2;
				}
				int sequanc2 = 0;
				for (int TSIndex = 0; TSIndex <= TimeSales2.Size(); TSIndex++)
				{
					if (TimeSales2[TSIndex].Sequence <= LastProssesedSequnce2)
						continue;
					
					p_LastSequance2 = TimeSales2[TSIndex].Sequence;
					if (TimeSales2[TSIndex].Type == SC_TS_BID)
						VolumeBidSecndSymbol += TimeSales2[TSIndex].Volume;

					if (TimeSales2[TSIndex].Type == SC_TS_ASK)
						VolumeAskSecndSympol += TimeSales2[TSIndex].Volume;
				}

				if (TimeSales2.Size() == TimeSales2.MaxSize())
					LastProssesedSequnce2 = p_LastSequance2;
				else
					LastProssesedSequnce2 = TimeSales2.GetArraySize();
			}
		#pragma endregion

			

		informationToSend.Clear();
		informationToSend.Format("%s;%s;%s;%.2f;%.2f;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld", symbol.GetChars(), account.GetChars(), PNL.GetChars(), PricePosition, courrentPrice, dAsk, dBid, sizePosition, positionIsClosed, volumeBid, volumeAsk, pullingStachkingAcumuleBid, pullingStachkingAcumuleAsk,VolumeBidSecndSymbol,VolumeAskSecndSympol);
		char* informationLength;

		

		informationLength = (char*)malloc(informationToSend.GetLength() * sizeof(char));

		for (int i = 0; i < informationToSend.GetLength(); i++)
			informationLength[i] = informationToSend[i];

		send(SocketPer, (char*)informationLength, informationToSend.GetLength(), 0);

		//char bufuer[1024];
		//	
		//recv(SocketPer, bufuer, sizeof(bufuer),0); 

		//SCString DataRecv;
		//DataRecv.Clear();
		//DataRecv.Format("Data recv = %s", bufuer); 
		//sc.AddMessageToLog(DataRecv, 0);

		free(informationLength);

	}

	#pragma endregion
}



void InitWinSock()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}
