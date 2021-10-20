#pragma once

UENUM()
enum class EDirection 
{
	ED_Up,
	ED_Down,
	ED_Left,
	ED_Right,

	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EDirection, EDirection::Count);

class PlacementHelper {
public:
	static TArray<EDirection> FindNeighbour(FIntVector& Position, TArray<FIntVector>& Collection, int32 UnitLength);
	static FIntVector GetOffsetFromDirection(EDirection& Direction, int32 UnitLength);
	static EDirection GetReverseDirection(EDirection& Direction);
	static FRotator AlignRotation(EDirection& Direction);
};
