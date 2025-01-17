#include "LockstepSimulation.h"

void FLockstepSimulation::Initialize()
{
	for(int i = 0; i < MAX_UNIT_COUNT; i++)
	{
		Units[i].UnitID = i;
		Units[i].Radius = 6_fx;
		Units[i].Position.X = 25_fx * (i / 32 - 16);
		Units[i].Position.Y = 25_fx * (i % 32 - 16);
		Units[i].Moving = false;
	}
}

void FLockstepSimulation::Tick()
{
	if (Units[0].Moving)
	{
		fixed DeltaTime = 1_fx / 20_fx;
	
		fixed MoveSpeed = 100_fx;

		FVec TargetPosition = Units[0].TargetPosition;

		TArray<int> NearbyUnits;
	
		for(int i = 1; i < MAX_UNIT_COUNT; i++)
		{
			FVec DiffPosition = Units[i].Position - Units[0].Position;

			fixed NearbyDistance = Units[i].Radius + Units[0].Radius + MoveSpeed * DeltaTime;

			if (fpm::abs(DiffPosition.X) <= NearbyDistance && fpm::abs(DiffPosition.Y) <= NearbyDistance)
			{
				NearbyUnits.Add(i);
				UE_LOG(LogTemp, Warning, TEXT("Found nearby unit"))
			}
		}

		fixed RemainingTime = DeltaTime;

		int PreviousSweepHitUnit = -1;
		int Iterations = 0;

		while (RemainingTime > 0_fx && Iterations < 2)
		{
			Iterations++;
		
			FVec TargetDifference = TargetPosition - Units[0].Position;

			if (length(TargetDifference) <= 10_fx)
			{
				Units[0].Moving = false;
				break;
			}
		
			FVec TargetDirection = normalize(TargetDifference);

			UE_LOG(LogTemp, Warning, TEXT("TargetDir: %f %f"), static_cast<float>(TargetDirection.X), static_cast<float>(TargetDirection.Y))

			if (PreviousSweepHitUnit != -1)
			{
				FVec PreviousSweepHitUnitDirection = normalize(Units[PreviousSweepHitUnit].Position - Units[0].Position);
				fixed PreviousSweepHitUnitDirectionPhi = atan2(PreviousSweepHitUnitDirection.Y, PreviousSweepHitUnitDirection.X);

				if ((TargetDirection | PreviousSweepHitUnitDirection) > 0_fx)
				{				
					if ((TargetDirection | RotatePlus90(PreviousSweepHitUnitDirection)) > 0_fx)
					{
						fixed Phi = phi(PreviousSweepHitUnitDirection) + fixed::half_pi() + 1_fx / 32_fx;
						TargetDirection = phiToDir(Phi);
					}
					else
					{
						fixed Phi = phi(PreviousSweepHitUnitDirection) - fixed::half_pi() - 1_fx / 32_fx;
						TargetDirection = phiToDir(Phi);
					}
				}
			}
		
			int ClosestSweepHitUnit = -1;
			fixed ClosestSweepHitTime = 1_fx;

			for(int NearbyUnitID : NearbyUnits)
			{			
				fixed SweepHitTime = CalculateSweep(TargetDirection, MoveSpeed * RemainingTime, Units[0].Position, Units[0].Radius, Units[NearbyUnitID].Position, Units[NearbyUnitID].Radius);

				if (SweepHitTime < ClosestSweepHitTime)
				{
					ClosestSweepHitTime = SweepHitTime;
					ClosestSweepHitUnit = NearbyUnitID;
				}
			}

			if (ClosestSweepHitUnit != -1)
			{
				Units[0].Position.X += ClosestSweepHitTime * MoveSpeed * RemainingTime * TargetDirection.X;
				Units[0].Position.Y += ClosestSweepHitTime * MoveSpeed * RemainingTime * TargetDirection.Y;
				RemainingTime -= RemainingTime * ClosestSweepHitTime;
				PreviousSweepHitUnit = ClosestSweepHitUnit;
			}
			else
			{
				Units[0].Position.X += MoveSpeed * RemainingTime * TargetDirection.X;
				Units[0].Position.Y += MoveSpeed * RemainingTime * TargetDirection.Y;
				RemainingTime = 0_fx;
				PreviousSweepHitUnit = -1;
			}
		}
	}
}

int FLockstepSimulation::GetUnitCount() const
{
	return MAX_UNIT_COUNT;
}

FVector2D FLockstepSimulation::GetUnitPosition(int UnitID) const
{
	auto Position = Units[UnitID].Position;
	return FVector2D(static_cast<float>(Position.X), static_cast<float>(Position.Y));
}

void FLockstepSimulation::ProcessMoveCommand(int X, int Y)
{
	Units[0].Moving = true;
	Units[0].TargetPosition = FVec{fixed::from_raw_value(X), fixed::from_raw_value(Y)};
}

fixed FLockstepSimulation::CalculateSweep(FVec Dir, fixed Distance, FVec P0, fixed R0, FVec P1, fixed R1)
{
	Dir = normalize(Dir);

	FVec DeltaP = P1 - P0;

	if ((DeltaP | Dir) <= 0_fx)
	{
		return 1_fx;
	}
	else
	{
		fixed B = 2 * (Dir | (-DeltaP));
		fixed C = (DeltaP | DeltaP) - (R0 + R1) * (R0 + R1);

		fixed Determinant = B * B - 4 * C;

		if (Determinant < 0_fx)
		{
			return 1_fx;
		}
		else
		{
			fixed Root0 = (-B - fpm::sqrt(Determinant)) / 2;
			fixed Root1 = (-B + fpm::sqrt(Determinant)) / 2;

			if (Root0 >= 0_fx)
			{
				return Root0 / Distance;
			}
			else if (0_fx >= (Root0+Root1)/2)
			{
				return 1_fx;
			}
			else
			{
				return 0_fx;
			}
		}
	}
}
