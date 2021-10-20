#include "PlacementHelper.h"

TArray<EDirection> PlacementHelper::FindNeighbour(FIntVector& Position, TArray<FIntVector>& Collection, int32 UnitLength)
{
    TArray<EDirection> NeighbourDirections = TArray<EDirection>();
    if (Collection.Contains(Position + FIntVector(0, UnitLength, 0)))
    {
        NeighbourDirections.Add(EDirection::ED_Right);
    }
    if (Collection.Contains(Position - FIntVector(0, UnitLength, 0)))
    {
        NeighbourDirections.Add(EDirection::ED_Left);
    }
    if (Collection.Contains(Position + FIntVector(UnitLength, 0, 0)))
    {
        NeighbourDirections.Add(EDirection::ED_Up);
    }
    if (Collection.Contains(Position - FIntVector(UnitLength, 0, 0)))
    {
        NeighbourDirections.Add(EDirection::ED_Down);
    }
    return NeighbourDirections;
}

FIntVector PlacementHelper::GetOffsetFromDirection(EDirection& Direction, int32 UnitLength)
{
    switch (Direction)
    {
    case EDirection::ED_Up:
        return FIntVector(UnitLength, 0, 0);
    case EDirection::ED_Down:
        return FIntVector(-UnitLength, 0, 0);
    case EDirection::ED_Left:
        return FIntVector(0, -UnitLength, 0);
    case EDirection::ED_Right:
        return FIntVector(0, UnitLength, 0);
    default:
        break;
    }
    return FIntVector(0);
}

EDirection PlacementHelper::GetReverseDirection(EDirection& Direction)
{
    switch (Direction)
    {
    case EDirection::ED_Up:
        return EDirection::ED_Down;
    case EDirection::ED_Down:
        return EDirection::ED_Up;
    case EDirection::ED_Left:
        return EDirection::ED_Right;
    case EDirection::ED_Right:
        return EDirection::ED_Left;
    default:
        break;
    }
    return Direction;
}

FRotator PlacementHelper::AlignRotation(EDirection& Direction)
{
    switch (Direction)
    {
    case EDirection::ED_Up:
        return FRotator(0.f, -90.f, 0.f);
    case EDirection::ED_Down:
        return FRotator(0.f, 90.f, 0.f);
    case EDirection::ED_Left:
        return FRotator(0.f, 180.f, 0.f);
    case EDirection::ED_Right:
        return FRotator(0.f, 0.f, 0.f);
    default:
        return FRotator(0.f);
    }
}