#pragma once
#include "CYGObject.h"
class CYGMonster :public CYGObject
{
public:
	CYGMonster();
	virtual ~CYGMonster() { Release(); }
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
	void OnCollision(CObject* _obj) override;
public:
	RECT Get_HitBox() { return m_tHitRect; }
public:
	void Set_Hp(int _i) { m_iHp += _i; }
private:
	//�� �տ� 
	D3DXVECTOR3 m_vLeftGunHandPos;
	D3DXVECTOR3 m_vRightGunHandPos;
	D3DXVECTOR3 m_vGunRectanglePoint[4];
	D3DXVECTOR3 m_vOriginLeftGunHand;
	D3DXVECTOR3 m_vOriginRightGunHand;
	D3DXVECTOR3 m_vOriginGunRectanglePoint[4];
	D3DXVECTOR3 m_vBulletSpawn;
	D3DXVECTOR3 m_vOriginBulletSpawn;

	//�÷��̾� ���� ����
	D3DXVECTOR3 m_vOriginPos;

	//�� ����
	int m_iShootTick;
};

