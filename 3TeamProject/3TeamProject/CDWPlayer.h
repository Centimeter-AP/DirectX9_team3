#pragma once
#include "CObject.h"
class CDWPlayer :public CObject
{
public:
	CDWPlayer();
	virtual ~CDWPlayer() { Release(); }
public:
	void Initialize() override;
	int Update() override;
	void Key_Input();
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
	void OnCollision(CObject* _obj) override;

private:
	D3DXVECTOR3     m_vPoint[4];
	D3DXVECTOR3     m_vOriginPoint[4];


	D3DXMATRIX        matScale;  // ũ��
	D3DXMATRIX        matRotZ; // ����
	D3DXMATRIX        matTrans; // ��ġ


	D3DXMATRIX        matScale_Sosil;  // ũ��
	D3DXMATRIX        matRotZ_Sosil; // ����
	D3DXMATRIX        matTrans_Sosil; // ��ġ

	D3DXMATRIX WorldMat;
	D3DXMATRIX WorldMat_Sosil;

	D3DXVECTOR3 transformedCorners[4]; // ��İ����ع��� ����

	D3DXVECTOR3 transformedSosil[4];


	D3DXVECTOR3 m_vCorner[4];
	D3DXVECTOR3 Sosil[4];

	float m_fNemoAngel;
};


