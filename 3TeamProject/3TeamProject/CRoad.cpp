#include "pch.h"
#include "CRoad.h"
#include "CKeyManager.h"
#include "CObjectManager.h"

bool CRoad::m_bLeft_Rotation = false;
bool CRoad::m_bRight_Rotation = false;

CRoad::CRoad()
{

}

void CRoad::Initialize()
{
	m_eOBJID = OBJ_DW_ROAD;
	m_eRender = RENDER_GAMEOBJECT;


	m_tInfo.vPos = { 400.f, 400.f, 0.f };
	m_fSpeed = 10.f;
	m_tInfo.vLook = { 1.f, 0.f, 0.f };
	m_fAngle = 0.f;
	m_fTargetAngle = 0.f; // �ʱ� ��ǥ ���� ����

	m_vCorner[0] = { -1500,-1500,0 }; // ������
	m_vCorner[1] = { 1500,-1500,0 }; // ��������
	m_vCorner[2] = { 1500,1500,0 }; // ������ �Ʒ�
	m_vCorner[3] = { -1500,1500,0 }; // ���� �Ʒ�

	Sosil[0] = { 350.f, 250.f, 0.f }; // ���� ��
	Sosil[1] = { 450.f, 250.f, 0.f }; // ������ ��
	Sosil[2] = { 450.f, 350.f, 0.f }; // ������ �Ʒ�
	Sosil[3] = { 350.f, 350.f, 0.f }; // ���� �Ʒ�



	m_pPlayer = CObjectManager::Get_Instance()->Get_ObjList_ByID(OBJ_PLAYER).front();

	m_bFirst_Check = false;
}


int CRoad::Update()
{
	Key_Input();

	if (m_bLeft_Rotation)
	{
			static float m_Move = 20.f;
			m_pPlayer->Set_Pos(m_pPlayer->Get_Info().vPos.x + m_Move, 550.f);
		
       if(m_pPlayer->Get_Info().vPos.x > 610)
		m_bLeft_Rotation = false;
	}

	if (m_bRight_Rotation)
	{
		//static float m_Move = 20.f;
		//m_pPlayer->Set_Pos(m_pPlayer->Get_Info().vPos.x - m_Move, 550.f);

		//if (m_pPlayer->Get_Info().vPos.x < 210)
		//	m_bRight_Rotation = false;
	}


	if (abs(m_fTargetAngle - m_fAngle) > D3DXToRadian(1.f)) 
	{
		if (m_fTargetAngle > m_fAngle)
			m_fAngle += D3DXToRadian(5.f);
		else if (m_fTargetAngle < m_fAngle)
			m_fAngle -= D3DXToRadian(5.f);
	}

	D3DXMatrixRotationZ(&matRotZ, m_fAngle);
	D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, m_tInfo.vPos.z);
	WorldMat = matRotZ * matTrans;

	for (int i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&transformedCorners[i], &m_vCorner[i], &WorldMat);
	}

	D3DXVECTOR3 sosilCenter =
	{
		(Sosil[0].x + Sosil[2].x) / 2.f,
		(Sosil[0].y + Sosil[2].y) / 2.f,
		0.f
	};

	float rotationAngle = m_fAngle;

	for (int i = 0; i < 4; i++)
	{
		D3DXVECTOR3 relativePoint = { Sosil[i].x - sosilCenter.x, Sosil[i].y - sosilCenter.y, 0.f };

		float rotatedX = relativePoint.x * cos(rotationAngle) - relativePoint.y * sin(rotationAngle);
		float rotatedY = relativePoint.x * sin(rotationAngle) + relativePoint.y * cos(rotationAngle);

		rotatedSosil[i].x = rotatedX + sosilCenter.x;
		rotatedSosil[i].y = rotatedY + sosilCenter.y;
		rotatedSosil[i].z = 0.f;
	}

	__super::Update_Rect();
	return 0;
}



void CRoad::Key_Input()
{

	if (GetAsyncKeyState('A'))
	{
		// ��θ� ��������
		D3DXVECTOR3 fixedLeftDir = { +10.f, 0.f, 0.f };
		m_tInfo.vPos += fixedLeftDir; 

		// ���߿� ù�浹�� 270��ǥ�� �ٲٵ�
		// ó�� ��� ��ġ��ǥ�� �ٲٵ� �� ����
		if (m_pPlayer->Get_Info().vPos.x < 320)
		{
			m_bLeft_Rotation = !m_bLeft_Rotation;
			m_fTargetAngle += D3DXToRadian(90.f); 
			m_tInfo.vPos = { 0.f, 0.f, 0.f };
			m_bRight_Rotation = true;
		}
	}


	else if (GetAsyncKeyState('D'))
	{
	     // ��θ� ����������
		D3DXVECTOR3 fixedRightDir = { -10.f, 0.f, 0.f };
		m_tInfo.vPos += fixedRightDir; 

	
	}

	// WŰ: ȭ�� ���� ����(+Y) �̵�
	if (GetAsyncKeyState('W'))
	{
		D3DXVECTOR3 fixedUpDir = { 0.f, -20.f, 0.f };
		m_tInfo.vPos += fixedUpDir;
	}

	// SŰ: ȭ�� ���� �Ʒ���(-Y) �̵�
	else if (GetAsyncKeyState('S'))
	{
		D3DXVECTOR3 fixedDownDir = { 0.f, 20.f, 0.f };
		m_tInfo.vPos += fixedDownDir;
	}

	// ���� �������� 90�� ȸ��
	if (CKeyManager::Get_Instance()->Key_Down(VK_LEFT))
	{
		m_bRight_Rotation = !m_bRight_Rotation;
		m_fTargetAngle -= D3DXToRadian(90.f);
		m_tInfo.vPos = { 0.f, 0.f, 0.f };
		m_bLeft_Rotation = true;
	}

	// ������ �������� 90�� ȸ��
	if (CKeyManager::Get_Instance()->Key_Down(VK_RIGHT))
	{
		m_bLeft_Rotation = !m_bLeft_Rotation;
		m_fTargetAngle += D3DXToRadian(90.f);
	}


	if (CKeyManager::Get_Instance()->Key_Pressing(VK_SPACE))
	{
		D3DXVECTOR3 fixedForwardDir = { 0.f, 40.f, 0.f };
		m_tInfo.vPos += fixedForwardDir;
	}
}

void CRoad::Late_Update()
{
}
void CRoad::Render(HDC hDC)
{
	// �÷��̾� �簢�� ������
	MoveToEx(hDC, transformedCorners[0].x, transformedCorners[0].y, nullptr);
	LineTo(hDC, transformedCorners[1].x, transformedCorners[1].y);
	LineTo(hDC, transformedCorners[2].x, transformedCorners[2].y);
	LineTo(hDC, transformedCorners[3].x, transformedCorners[3].y);
	LineTo(hDC, transformedCorners[0].x, transformedCorners[0].y);

	// �ҽ��� �׸� ������
	MoveToEx(hDC, rotatedSosil[0].x, rotatedSosil[0].y, nullptr);
	LineTo(hDC, rotatedSosil[1].x, rotatedSosil[1].y);
	LineTo(hDC, rotatedSosil[2].x, rotatedSosil[2].y);
	LineTo(hDC, rotatedSosil[3].x, rotatedSosil[3].y);
	LineTo(hDC, rotatedSosil[0].x, rotatedSosil[0].y);

	// �÷��̾� �簢���� �ҽ��� �׸� ����
	for (int i = 0; i < 4; ++i)
	{
		MoveToEx(hDC, transformedCorners[i].x, transformedCorners[i].y, nullptr);
		LineTo(hDC, rotatedSosil[i].x, rotatedSosil[i].y);
	}


	TCHAR m_szBuf[100] = {};
	swprintf_s(m_szBuf, L"��� x : %.f, ��� y : %.f", m_tInfo.vPos.x, m_tInfo.vPos.y);
	TextOut(hDC, 300, 30, m_szBuf, lstrlen(m_szBuf));



	if (g_bDevmode)
	{
		HitCircle(hDC, m_tHitRect, 0, 0);
	}


}


void CRoad::Release()
{

}

void CRoad::OnCollision(CObject* _obj)
{
}
bool CheckCollision(D3DXVECTOR3 playerPos, D3DXVECTOR3 lineStart, D3DXVECTOR3 lineEnd, float threshold)
{
    // ���� ����
    D3DXVECTOR3 lineVec = lineEnd - lineStart;

    // ���� ����
    float lineLength = D3DXVec3Length(&lineVec);

    // ���� �÷��̾� ������ �Ÿ� ���
    float distance = abs((lineEnd.y - lineStart.y) * playerPos.x -
                         (lineEnd.x - lineStart.x) * playerPos.y +
                         lineEnd.x * lineStart.y - lineEnd.y * lineStart.x) / lineLength;

    return distance <= threshold; // �÷��̾� ũ��(threshold) ���϶�� �浹
}
