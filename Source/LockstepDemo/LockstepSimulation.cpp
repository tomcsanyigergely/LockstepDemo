#include "LockstepSimulation.h"

#include "LockstepGameStateBase.h"

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
	for (int k = 0; k < MAX_UNIT_COUNT; k++)
	{
		FUnit& CurrentUnit = Units[k];
		
		if (CurrentUnit.Moving)
		{
			fixed DeltaTime = 1_fx / 20_fx;
	
			fixed MoveSpeed = 100_fx;

			FVec TargetPosition = CurrentUnit.TargetPosition;

			TArray<int> NearbyUnits;
		
			for(int i = 0; i < MAX_UNIT_COUNT; i++)
			{
				if (i != k)
				{
					FVec DiffPosition = Units[i].Position - CurrentUnit.Position;

					fixed NearbyDistance = Units[i].Radius + CurrentUnit.Radius + MoveSpeed * DeltaTime;

					if (fpm::abs(DiffPosition.X) <= NearbyDistance && fpm::abs(DiffPosition.Y) <= NearbyDistance)
					{
						NearbyUnits.Add(i);
					}
				}
			}

			fixed RemainingTime = DeltaTime;

			int PreviousSweepHitUnit = -1;
			int Iterations = 0;

			while (RemainingTime > 0_fx && Iterations < 2)
			{
				Iterations++;
			
				FVec TargetDifference = TargetPosition - CurrentUnit.Position;

				if (length(TargetDifference) <= 10_fx)
				{
					CurrentUnit.Moving = false;
					break;
				}
			
				FVec TargetDirection = normalize(TargetDifference);

				if (PreviousSweepHitUnit != -1)
				{
					FVec PreviousSweepHitUnitDirection = normalize(Units[PreviousSweepHitUnit].Position - CurrentUnit.Position);
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
					fixed SweepHitTime = CalculateSweep(TargetDirection, MoveSpeed * RemainingTime, CurrentUnit.Position, CurrentUnit.Radius, Units[NearbyUnitID].Position, Units[NearbyUnitID].Radius);

					if (SweepHitTime < ClosestSweepHitTime)
					{
						ClosestSweepHitTime = SweepHitTime;
						ClosestSweepHitUnit = NearbyUnitID;
					}
				}

				if (ClosestSweepHitUnit != -1)
				{
					CurrentUnit.Position.X += ClosestSweepHitTime * MoveSpeed * RemainingTime * TargetDirection.X;
					CurrentUnit.Position.Y += ClosestSweepHitTime * MoveSpeed * RemainingTime * TargetDirection.Y;
					RemainingTime -= RemainingTime * ClosestSweepHitTime;
					PreviousSweepHitUnit = ClosestSweepHitUnit;
				}
				else
				{
					CurrentUnit.Position.X += MoveSpeed * RemainingTime * TargetDirection.X;
					CurrentUnit.Position.Y += MoveSpeed * RemainingTime * TargetDirection.Y;
					RemainingTime = 0_fx;
					PreviousSweepHitUnit = -1;
				}
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

void FLockstepSimulation::ProcessMoveCommand(const FLockstepMoveCommand& MoveCommand)
{
	FVec SelectedPosition = FVec{fixed::from_raw_value(MoveCommand.X), fixed::from_raw_value(MoveCommand.Y)};
	
	FVec UnitCenterPosition{0_fx, 0_fx};

	fixed UnitCount = fixed(MoveCommand.Units.Num());

	for(uint16 UnitID : MoveCommand.Units)
	{		
		UnitCenterPosition.X += (Units[UnitID].Position.X / UnitCount);
		UnitCenterPosition.Y += (Units[UnitID].Position.Y / UnitCount);
	}

	FVec Delta = SelectedPosition - UnitCenterPosition;

	for(uint16 UnitID : MoveCommand.Units)
	{
		Units[UnitID].Moving = true;
		Units[UnitID].TargetPosition = Units[UnitID].Position + Delta;
	}
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
