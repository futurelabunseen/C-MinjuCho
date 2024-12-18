// Fill out your copyright notice in the Description page of Project Settings.
/*
 * 이 코드는 오브젝트 풀링 시스템으로,
 * 총알 액터나 UI 요소 같은 재사용 가능한 오브젝트를 효율적으로 관리하기 위해 설계되었습니다.
 *
 * [설계 의도]
 * 이 시스템의 주된 목표는 오브젝트를 자주 생성하고 파괴하는 대신, 재사용함으로써 성능을 최적화하는 것입니다.
 * 템플릿과 인터페이스를 사용하여 다양한 오브젝트 타입을 지원하며, 비활성화된 오브젝트와 활성화된 오브젝트를 풀에 저장한 후 필요할 때마다 확장할 수 있습니다.
 * 이 설계는 액터뿐만 아니라 UI 요소 등 다양한 게임 요소에 적용할 수 있어 범용성이 높습니다.
 * GameInstanceSubsystem을 상속받았기에, 레벨을 이동해도 멤버변수에 남아 풀이 계속 유지되는 것을 의도했습니다.
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interface/CMPoolableInterface.h"
#include "Blueprint/UserWidget.h"
#include "CMObjectPoolManager.generated.h"

USTRUCT()
struct FGenericObjectPool
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UObject> ClassInfo;
	UPROPERTY()
	TArray<UObject*> DeactiveObjects;
	UPROPERTY()
	TArray<UObject*> ActiveObjects;
};

/**
 * 
 */
UCLASS()
class COLORMONSTER_1_API UCMObjectPoolManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UCMObjectPoolManager();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override {}

	template <typename T>
	void CreatePool(TSubclassOf<T> InObjectClass, int32 InPoolSize);

	template <typename T>
	TObjectPtr<T> GetPooledObject();

	// 자동화 필요
	template <typename T>
	void ReturnPooledObject(TObjectPtr<T> InObject);

	private:

	template <typename T>
	void ExpandPool(FGenericObjectPool* InPool, TSubclassOf<T> InObjectClass, int32 InPoolSize);

	template <typename T>
	TObjectPtr<T> PopPooledObject(FGenericObjectPool* InPool);

	TMap<FName, FGenericObjectPool*> ObjectPools;

	UPROPERTY()
	int32 DefaultPoolSize = 10;

};

template<typename T>
void UCMObjectPoolManager::CreatePool(TSubclassOf<T> InObjectClass, int32 InPoolSize)
{
	// Check Reset Implemented
	TObjectPtr<T> ObjForCheck = NewObject<T>(this, InObjectClass);
	ICMPoolableInterface* InterfaceObj = Cast<ICMPoolableInterface>(ObjForCheck);
	if (InterfaceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Class is not Poolable"));
		return;
	}
	// Already Pooled
	if (ObjectPools.Contains(T::StaticClass()->GetFName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Already Pooled"));
		return;
	}
	// Correct Size
	int32 PoolSize = InPoolSize;
	if (InPoolSize <= 0)
	{
		PoolSize = DefaultPoolSize;
	}
	FGenericObjectPool* NewPool = new FGenericObjectPool();
	NewPool->ClassInfo = InObjectClass;
	ExpandPool<T>(NewPool, InObjectClass, PoolSize);
	ObjectPools.Add(T::StaticClass()->GetFName(), NewPool);
	UE_LOG(LogTemp, Warning, TEXT("CreatePool Good"));
}

template<typename T>
TObjectPtr<T> UCMObjectPoolManager::GetPooledObject()
{
	FGenericObjectPool** CheckPooledInfo = ObjectPools.Find(T::StaticClass()->GetFName());
	if (CheckPooledInfo == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("It has no Pool"));
		// CreatePool 자동으로 호출하게 할까?
		return nullptr;
	}
	FGenericObjectPool* PooledInfo = *CheckPooledInfo;
	// Lack
	if (ObjectPools[T::StaticClass()->GetFName()]->DeactiveObjects.Num() <= 0)
	{
		ExpandPool<T>(PooledInfo, T::StaticClass(), DefaultPoolSize);
	}
	TObjectPtr<T> PopObj = PopPooledObject<T>(PooledInfo);
	ensure(PopObj);
	PooledInfo->ActiveObjects.Add(PopObj);

	UE_LOG(LogTemp, Warning, TEXT("Pooled Object: %s"), *(PopObj->GetName()));
	UE_LOG(LogTemp, Warning, TEXT("GetPooledObject Good"));
	return PopObj;
}

template<typename T>
void UCMObjectPoolManager::ReturnPooledObject(TObjectPtr<T> InObject)
{
	// Check Hide Implemented
	TObjectPtr<T> ObjForCheck = NewObject<T>(this, T::StaticClass());
	ICMPoolableInterface* InterfaceObj = Cast<ICMPoolableInterface>(ObjForCheck);
	if (InterfaceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Class is not Poolable"));
		return;
	}
	FGenericObjectPool** CheckPooledInfo = ObjectPools.Find(T::StaticClass()->GetFName());
	if (CheckPooledInfo == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("It has no Pool"));
		return;
	}
	FGenericObjectPool* PooledInfo = *CheckPooledInfo;
	if (PooledInfo->ActiveObjects.Contains(InObject) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of ActivePool"));
		return;
	}
	int32 FindIdx = PooledInfo->ActiveObjects.Find(InObject);
	TObjectPtr<T> DeletedObj = Cast<T>(PooledInfo->ActiveObjects[FindIdx]);
	ensure(DeletedObj);
	DeletedObj->Hide();
	PooledInfo->DeactiveObjects.Add(DeletedObj);
	UE_LOG(LogTemp, Warning, TEXT("Pooled Object: %s"), *(DeletedObj->GetName()));
	UE_LOG(LogTemp, Warning, TEXT("ReturnPooledObject  Good"));
}

template<typename T>
void UCMObjectPoolManager::ExpandPool(FGenericObjectPool* InPool, TSubclassOf<T> InObjectClass, int32 InPoolSize)
{
	// Check Hide Implemented
	TObjectPtr<T> ObjForCheck = NewObject<T>(this, InObjectClass);
	ICMPoolableInterface* InterfaceObj = Cast<ICMPoolableInterface>(ObjForCheck);
	if (InterfaceObj == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Class is not Poolable"));
		return;
	}

	for (int32 i = 0; i < InPoolSize; ++i)
	{
		TObjectPtr<T> TempObject = nullptr;
		/*if (InObjectClass->IsChildOf(UUserWidget::StaticClass()))
		{
			TObjectPtr<T> NewWidget = Cast<T>(CreateWidget(this, InObjectClass));
			TempObject = NewWidget;
		}*/
		if (InObjectClass->IsChildOf(AActor::StaticClass()))
		{
			TObjectPtr<T> NewActor = Cast<T>(GetWorld()->SpawnActor<AActor>(InObjectClass));
			TempObject = NewActor;
		}
		else
		{
			TempObject = NewObject<T>(InObjectClass);
		}
		TempObject->Hide();
		InPool->DeactiveObjects.Add(TempObject);
	}
}

template<typename T>
TObjectPtr<T> UCMObjectPoolManager::PopPooledObject(FGenericObjectPool* InPool)
{
	int32 LastIdx = InPool->DeactiveObjects.Num() - 1;
	TObjectPtr<T> PopObj = Cast<T>(InPool->DeactiveObjects[LastIdx]);
	ensure(PopObj);
	InPool->DeactiveObjects.RemoveAt(LastIdx);
	return PopObj;
}
