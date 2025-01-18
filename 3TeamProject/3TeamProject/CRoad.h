#pragma once

#include "CDWObject.h"
#include <d3dx9math.h>  
class CRoad : public CDWObject
{
	struct SObstacle
	{
		D3DXVECTOR3 vPos;   // ��ֹ��� ���� ��ġ (z�� ����)
		float       fWidth; // ��ֹ��� ����ũ��
		float       fHeight;// ��ֹ��� ����ũ��
		float       fAngle; // Z�� ���� ȸ���� (2D�����̸� yaw ����)
		float       fScale; // ������(����)
	};



public:
	CRoad();
	virtual ~CRoad() { Release(); }
public:
	void Initialize() override;
	int Update() override;
	void Key_Input();
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
	void OnCollision(CObject* _obj) override;
    void Wall_Update();

	void Update_Obstacles();

	void Render_Obstacles(HDC hDC);

private:
	D3DXVECTOR3     m_vPoint[4];
	D3DXVECTOR3     m_vOriginPoint[4];


	D3DXMATRIX        matScale_Sosil;  // ũ��
	D3DXMATRIX        matRotZ_Sosil; // ����
	D3DXMATRIX        matTrans_Sosil; // ��ġ

	D3DXMATRIX WorldMat_Sosil;

	D3DXVECTOR3 transformedCorners[4]; // ��İ����ع��� ����

	D3DXVECTOR3 rotatedSosil[4];


	D3DXVECTOR3 m_vCorner[4];
	D3DXVECTOR3 Sosil[4];



	float m_fTargetAngle;


	static bool m_bLeft_Rotation;
	static bool m_bRight_Rotation;
	static bool m_bTop_Rotation;
	static bool m_bBottom_Rotation;

	bool m_bFirst_Check;

	CObject* m_pPlayer;

	vector<POINT*> m_vecPoints; // ���;ȿ� ����Ʈ��� 4��¥���迭��`

	
	vector<SObstacle> m_Obstacles;


};

