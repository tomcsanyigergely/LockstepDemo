#pragma once
#include "FixedPoint.h"

class FLockstepSimulation 
{
    struct FUnit
    {
        uint16 UnitID;
        fixed Radius;
        uint16 CellID;
        FVec Position;
        bool Moving;
        FVec TargetPosition;
    };

    struct FCell
    {
        TSet<uint16> Units;
    };

    static constexpr int MAX_UNIT_COUNT = 1024;

    FUnit Units[MAX_UNIT_COUNT];
    FCell Cells[640];
    
public:
    void Initialize();
    
    void Tick();

    int GetUnitCount() const;

    FVector2D GetUnitPosition(int UnitID) const;

    void ProcessMoveCommand(int X, int Y);

private:
    static fixed CalculateSweep(FVec Dir, fixed Distance, FVec P0, fixed R0, FVec P1, fixed R1);
};
