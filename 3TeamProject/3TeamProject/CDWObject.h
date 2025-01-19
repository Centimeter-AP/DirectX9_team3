#pragma once
#include "CObject.h"

class CDWObject : public CObject
{
public:
	CDWObject();
	virtual ~CDWObject();
public:
	void Initialize() PURE;
	int Update() PURE;
	void Late_Update() PURE;
	void Render(HDC hDC) PURE;
	void Release() PURE;
	void OnCollision(CObject* _obj) PURE;
	void Initialize_OriginPoint(const int& _iCount, const int& _Radius, const float& _fStartAngle = 0.f);
	vector<D3DXVECTOR3>& Get_World_Point()
	{
		return m_vPointvec;
	}
	auto Get_RenderPoint()
	{
		return m_pRenderPoint;
	}
	D3DXMATRIX        matScale;  // ũ��
	D3DXMATRIX        matRotZ; // ����
	D3DXMATRIX        matTrans; // ��ġ
	D3DXMATRIX WorldMat; //���� ���

	float m_fAngle;

protected:
	vector<D3DXVECTOR3> m_vOriginPointvec; // ���� ��ǥ
	vector<D3DXVECTOR3> m_vPointvec; // ���� ��ȯ ���

	float				m_fRadius;

	POINT* m_pRenderPoint;

};

