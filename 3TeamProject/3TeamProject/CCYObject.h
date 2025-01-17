#pragma once
#include "CObject.h"
class CCYObject : public CObject
{
public:
	CCYObject(): m_pRenderPoint(nullptr), m_fRadius(0.f) {}
	virtual ~CCYObject();
public:
	void Initialize() PURE;
	int Update() PURE;
	void Late_Update() PURE;
	void Render(HDC hDC) PURE;
	void Release() PURE;
	void OnCollision(CObject* _obj) PURE;

	void Initialize_OriginPoint(const int& _iCount, const int& _Radius, const float& _fStartAngle = 0.f);
	void Set_PositionVector(D3DXVECTOR3 _vPos) { m_tInfo.vPos = _vPos; }
	void Set_DirectionVector(D3DXVECTOR3 _vDir) { m_tInfo.vDir = _vDir; }
	D3DXVECTOR3& Get_DirectionVector() { return m_tInfo.vDir; }

protected:
	vector<D3DXVECTOR3> m_vOriginPointvec;
	vector<D3DXVECTOR3> m_vPointvec;
	POINT*				m_pRenderPoint;
	float				m_fRadius;

	D3DXMATRIX			matScale;
	D3DXMATRIX			matRotZ;
	D3DXMATRIX			matTrans;
	float				m_fAngle;
};

