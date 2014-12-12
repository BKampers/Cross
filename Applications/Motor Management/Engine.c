#include "Engine.h"

#include <stdint.h>
#include <math.h>

#include "PersistentElementManager.h"
#include "Ignition.h"

/*
** Private 
*/


typedef struct
{
    uint8_t cogTotal;
    uint8_t gapSize;
    uint8_t offset;
} Cogwheel;


typedef struct
{
    Cogwheel cogwheel;
    uint8_t cylinderCount;
} Engine;


Engine engine;
TypeId engineTypeId = INVALID_TYPE_ID;


int ValidCog(int number)
{
    int total = GetCogTotal();
    int effective = GetEffectiveCogCount();
    number %= total;
    if (number == 0)
    {
        return 1;
    }
    else if (number <= effective)
    {
        return number;
    }
    else if (total - number < number - effective)
    {
        return 1;
    }
    else
    {
        return effective;
    }
}


Status StoreEngine()
{
    Status status = InitIgnitionStartCogs();
    if (status == OK)
    {
        Reference reference;
        status = FindFirst(engineTypeId, &reference);
        if (status == OK)
        {
            status = StoreElement(&engine, reference, sizeof(engine));
        }
    }
    return status;
}


/*
** Interface
*/

void RegisterEngineType()
{
    RegisterType(&engineTypeId);
}


Status InitEngine()
{
    Reference reference;
    Status status = FindFirst(engineTypeId, &reference);
    if (status == OK)
    {
        status = GetElement(reference, sizeof(engine), &engine);
    }
    else
    {
        engine.cogwheel.cogTotal = 60;
        engine.cogwheel.gapSize = 2;
        engine.cogwheel.offset = 20;
        engine.cylinderCount = 4;
        status = AllocateElement(engineTypeId, sizeof(engine), &reference);
        if (status == OK)
        {
            status = StoreElement(&engine, reference, sizeof(engine));
        }
    }
    return status;
}


int GetCogTotal()
{
    return engine.cogwheel.cogTotal;
}


int GetGapSize()
{
    return engine.cogwheel.gapSize;
}


int GetEffectiveCogCount()
{
    return engine.cogwheel.cogTotal - engine.cogwheel.gapSize;
}


int GetDeadPointOffset()
{
    return engine.cogwheel.offset;
}


int GetDeadPointCount()
{
    switch (engine.cylinderCount)
    {
        case 8:
            return 4;
        case 6:
            return 3;
        default:
            return 2;
    }
}


int GetDeadPointCog(int index)
{
    return ValidCog(engine.cogwheel.offset + roundf((float) engine.cogwheel.cogTotal / GetDeadPointCount() * index));
}


int GetCylinderCount()
{
    return engine.cylinderCount;
}


Status SetGogwheelProperties(int cogTotal, int gapSize, int offset)
{
    if ((2 <= cogTotal) && (cogTotal <= 250) && (1 <= gapSize) && (gapSize < cogTotal) && (1 <= offset) && (offset < cogTotal - gapSize))
    {
        engine.cogwheel.cogTotal = (uint8_t) cogTotal;
        engine.cogwheel.gapSize = (uint8_t) gapSize;
        engine.cogwheel.offset = (uint8_t) offset;
        return StoreEngine();
    }
    else
    {
        return "InvalidCogwheelProperties";
    }
}


Status SetCylinderCount(int count)
{
    if ((count == 4) || (count == 6) || (count == 8))
    {
        engine.cylinderCount = (uint8_t) count;
        return StoreEngine();
    }
    else
    {
        return "InvalidCylinderCount";
    }
}