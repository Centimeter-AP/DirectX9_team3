#include "pch.h"
#include "CJWScene.h"
#include "CKeyManager.h"
#include "CSceneManager.h"
#include "CUiManager.h"
#include "CAbstractFactory.h"
#include "CObjectManager.h"
#include "CCollisionManager.h"
#include "CSoundManager.h"
#include <time.h>

CJWScene::CJWScene() :m_pFruit(nullptr), m_iLevel(0), m_bCreated(false), m_fDropLenDebug(0.f), m_bCanWarning(false), m_bGameOver(false), m_iScore(0),
m_dwDropDelay(0ULL), m_dwDroppedTime(0ULL), m_tNextFruitInfo{}
{
	for (int i = 0; i < (int)FRUIT_TYPE::FT_END; i++)
	{
		m_FruitPoolMap[(FRUIT_TYPE)i] = nullptr;
	}
	m_vecScore = { 6,10,15,21,28,36,45,55,66 };
}

void CJWScene::Initialize()
{
	if (AddFontResourceEx(L"../Assets/Fonts/Eulyoo1945-SemiBold.otf", FR_PRIVATE, NULL) == 0)
	{
		MessageBox(NULL, L"��Ʈ �ε� ����", L"����", MB_OK);
	}
	
	CSoundManager::GetInstance()->PlayBGM("WaterMelon_BGM");
	CUiManager::Get_Instance()->Set_UiType(UI_JW);
	srand(unsigned int(time(nullptr)));
	m_bGameOver = false;
	m_bCanWarning = false;
	m_dwDropDelay = 1000ULL;
	m_dwDroppedTime = GetTickCount64();
	m_bCreated = true;
	m_iLevel = 0;
	m_iScore = 0;
	for (int i = 0; i < (int)FRUIT_TYPE::FT_END; i++)
	{
		if (!m_FruitPoolMap[(FRUIT_TYPE)i])
		{
			m_FruitPoolMap[(FRUIT_TYPE)i] = new CObjPool<CFruit>(20);
		}
	}

	FRUIT_TYPE eType = (FRUIT_TYPE)(rand() % (m_iLevel+1));
	CObject* pObj = m_FruitPoolMap[eType]->Get_Obj();
	static_cast<CFruit*>(pObj)->Set_Type(eType);
	static_cast<CFruit*>(pObj)->Reset();
	CObjectManager::Get_Instance()->Add_Object(OBJ_PLAYER, pObj);


	// ���� ���� ��
	eType = (FRUIT_TYPE)(rand() % (m_iLevel + 1));
	m_pFruit = m_FruitPoolMap[eType]->Get_Obj();
	static_cast<CFruit*>(m_pFruit)->Set_Type(eType);
	static_cast<CFruit*>(m_pFruit)->Reset();
	Update_Next_FruitInfo(static_cast<CFruit*>(m_pFruit)->Get_RenderPoints(), static_cast<CFruit*>(m_pFruit)->Get_Color(), static_cast<CFruit*>(pObj)->Get_FruitType());

}

int CJWScene::Update()
{
	Key_Input();
	if (!m_bGameOver)
	{
		auto& fruitList = CObjectManager::Get_Instance()->Get_ObjList_ByID(OBJ_PLAYER);
		CCollisionManager::JW_Collision_Circle(fruitList, fruitList);
		Create_MapObj();
		Merge_Fruit();
		BoxLine_Collision();
		Find_Proximate_Fruit();
		CObjectManager::Get_Instance()->Update();
	}
	return 0;
}

void CJWScene::Late_Update()
{
	if (!m_bGameOver)
	{
		Check_GameOver();
	}
	CObjectManager::Get_Instance()->Late_Update();
}

void CJWScene::Render(HDC hDC)
{
	HBRUSH ivoryBrush = CreateSolidBrush(RGB(255, 248, 235));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, ivoryBrush);
	Rectangle(hDC, 0, 0, WINCX, WINCY);
	Render_Box(hDC);
	if (g_bDevmode) {
		TCHAR szWhoScene[64];
		_stprintf_s(szWhoScene, _T("���"));
		SetTextColor(hDC, RGB(0, 0, 0));
		//SetBkMode(hDC, TRANSPARENT);
		TextOut(hDC, 300, 10, szWhoScene, _tcslen(szWhoScene));

		TCHAR szDebugScore[64];
		_stprintf_s(szDebugScore, _T("���� ���ھ� : %d"), m_iScore);
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, 50,30, szDebugScore, _tcslen(szDebugScore));

		TCHAR szDebugLevel[64];
		_stprintf_s(szDebugLevel, _T("���� ���� : %d"), m_iLevel);
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, 50, 50, szDebugLevel, _tcslen(szDebugLevel));

		TCHAR szDebugGameOver[64];
		_stprintf_s(szDebugGameOver, _T("���� ���� ���� : %d"), m_bGameOver);
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, 50, 70, szDebugGameOver, _tcslen(szDebugGameOver));

	

		HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
		// �� �׸���
		MoveToEx(hDC, 70, 70, nullptr);
		LineTo(hDC, 720, 70);
	
		// ���ҽ� ����
		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
	}

	TCHAR szDebugWarn[64];
	_stprintf_s(szDebugWarn, _T("������ ��� : %f"), m_fDropLenDebug);
	SetTextColor(hDC, RGB(0, 0, 0));
	TextOut(hDC, 50, 100, szDebugWarn, _tcslen(szDebugWarn));
	if (m_bCanWarning)
	{
		//HPEN hWarningPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));  // ������ ���
		//HPEN hOldPen = (HPEN)SelectObject(hDC, hWarningPen);

		//MoveToEx(hDC, 100, 70, nullptr);
		//LineTo(hDC, 700, 70);

		//SelectObject(hDC, hOldPen);
		//DeleteObject(hWarningPen);

		 // �������� �迭 (����, ��Ȳ, ���, �ʷ�, �Ķ�, ����, ����)
		vector<COLORREF> vecRainbow = {
			RGB(255, 0, 0),    // ����
			RGB(255, 165, 0),  // ��Ȳ
			RGB(255, 255, 0),  // ���
			RGB(0, 255, 0),    // �ʷ�
			RGB(0, 0, 255),    // �Ķ�
			RGB(75, 0, 130),   // ����
			RGB(148, 0, 211)   // ����
		};

		int startX = 70;   // ���� X ��ǥ
		int endX = 720;     // �� X ��ǥ
		int y = 70;         // Y ��ǥ
		int segmentLength = (endX - startX) / (int)vecRainbow.size();  // �� ������ �� ����

		HPEN hOldPen = nullptr;

		for (int i = 0; i < (int)vecRainbow.size(); ++i)
		{
			// ���� �� ����
			HPEN hPen = CreatePen(PS_SOLID, 3, vecRainbow[i]);
			hOldPen = (HPEN)SelectObject(hDC, hPen);

			// �� �׸���
			MoveToEx(hDC, startX + i * segmentLength, y, nullptr);
			LineTo(hDC, startX + (i + 1) * segmentLength, y);

			// �� ����
			SelectObject(hDC, hOldPen);
			DeleteObject(hPen);
		}
	}

	SelectObject(hDC, oldBrush);
	DeleteObject(ivoryBrush);
	CObjectManager::Get_Instance()->Render(hDC);
	CUiManager::Get_Instance()->Render(hDC);
}

void CJWScene::Release()
{
	if (m_pFruit)
	{
		// Safe_Delete(m_pFruit) ����
		m_FruitPoolMap[static_cast<CFruit*>(m_pFruit)->Get_FruitType()]->Return_Obj(static_cast<CFruit*>(m_pFruit));
		m_pFruit = nullptr;
	}
	CObjectManager::Get_Instance()->Release();
	CObjectManager::Get_Instance()->RenderList_Clear();
	for (auto& pool : m_FruitPoolMap)
	{
		Safe_Delete(pool.second);
	}
	m_FruitPoolMap.clear();

	CSoundManager::GetInstance()->StopAllSounds();
}

void CJWScene::Key_Input()
{
	if (CKeyManager::Get_Instance()->Key_Down(VK_F1)) {
		g_bDevmode = !g_bDevmode;
	}

	if (CKeyManager::Get_Instance()->Key_Down(VK_F9)) {
		CSceneManager::Get_Instance()->Set_Scene(SC_MENU);
	}

	if (CKeyManager::Get_Instance()->Key_Down(VK_LBUTTON))
	{
		if (m_bCreated)
		{
			m_dwDroppedTime = GetTickCount64();
			m_bCreated = false;
		}
	}
}

void CJWScene::Create_MapObj()
{
	if (!m_bCreated)
	{
		auto dwCur = GetTickCount64();
		if (dwCur - m_dwDroppedTime >= m_dwDropDelay)
		{
			if (!m_pFruit) return;
			m_bCreated = true;
		//	FRUIT_TYPE eType = (FRUIT_TYPE)(rand() % m_iLevel);
			//CObject* pObj = m_FruitPoolMap[eType]->Get_Obj();
			//static_cast<CFruit*>(pObj)->Set_Type(eType);
			static_cast<CFruit*>(m_pFruit)->Reset();
		//	Update_Next_FruitInfo(static_cast<CFruit*>(pObj)->Get_RenderPoints(), static_cast<CFruit*>(pObj)->Get_Color(), static_cast<CFruit*>(pObj)->Get_FruitType());
			CObjectManager::Get_Instance()->Add_Object(OBJ_PLAYER, m_pFruit);


			FRUIT_TYPE eType = (FRUIT_TYPE)(rand() % (m_iLevel + 1));
			m_pFruit = m_FruitPoolMap[eType]->Get_Obj();
			static_cast<CFruit*>(m_pFruit)->Set_Type(eType);
			static_cast<CFruit*>(m_pFruit)->Reset();
			Update_Next_FruitInfo(static_cast<CFruit*>(m_pFruit)->Get_RenderPoints(), static_cast<CFruit*>(m_pFruit)->Get_Color(), static_cast<CFruit*>(m_pFruit)->Get_FruitType());
		}
	}
}

void CJWScene::Offset()
{
}

void CJWScene::Check_GameOver()
{
	auto& fruitList = CObjectManager::Get_Instance()->Get_ObjList_ByID(OBJ_PLAYER);
	for (const auto& fruit : fruitList)
	{
		CFruit* pFruit = dynamic_cast<CFruit*>(fruit);

		if (pFruit && pFruit->Is_Active())
		{
			float fLen = (pFruit->Get_Info().vPos.y - pFruit->Get_Scale().y * pFruit->Get_Radius());
			if (fLen <= 70.f && pFruit->Is_Dropped() && (pFruit->In_Box()|| pFruit->Is_Collide()))
			{
				m_bGameOver = true;
			}
		}
	}
}

void CJWScene::BoxLine_Collision()
{
	auto& fruitList = CObjectManager::Get_Instance()->Get_ObjList_ByID(OBJ_PLAYER);
	for (const auto& fruit : fruitList)
	{
		CFruit* pFruit = dynamic_cast<CFruit*>(fruit);
		if (pFruit && !pFruit->In_Box()&& pFruit->Is_Dropped())
		{
			float fLen = pFruit->Get_Info().vPos.y - pFruit->Get_Radius() * pFruit->Get_Scale().y;
			pFruit->Set_LinePassed(fLen > 70.f);
		}
	}

}

void CJWScene::Merge_Fruit()
{
	FRUIT_TYPE eFruitType = FRUIT_TYPE::FT_END;
	FRUIT_STATE eMergeType = FRUIT_STATE::FS_END;
	auto& fruitList = CObjectManager::Get_Instance()->Get_ObjList_ByID(OBJ_PLAYER);
	for (auto it = fruitList.begin(); it != fruitList.end();)
	{
		CFruit* pFruit = dynamic_cast<CFruit*>(*it);
		if (!pFruit) continue;

		if (!pFruit->Is_Active())
		{
			eFruitType = FRUIT_TYPE((int)pFruit->Get_FruitType() + 1);
			eMergeType = pFruit->Get_State();
			if (eFruitType < FRUIT_TYPE::FT_END && eMergeType == FRUIT_STATE::MERGED)
			{
				m_iScore +=m_vecScore[(int)eFruitType - 1];
				m_iLevel = max(m_iLevel, (int)eFruitType);
				CFruit* pMergeFruit = m_FruitPoolMap[eFruitType]->Get_Obj();
				pMergeFruit->Set_Pos(pFruit->Get_Info().vPos.x, pFruit->Get_Info().vPos.y);
				pMergeFruit->Set_Type(eFruitType);
				pMergeFruit->Set_Merged_Fruit();
				CSoundManager::GetInstance()->PlayEffect("MergeSound");
				CObjectManager::Get_Instance()->Add_Object(OBJ_PLAYER, pMergeFruit);
			}
			m_FruitPoolMap[pFruit->Get_FruitType()]->Return_Obj(pFruit);
			it = fruitList.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CJWScene::Render_Box(HDC hDC)
{
	// �޸� (���� ����)
	POINT backPoints[] = {
		{100, 50}, {700, 50}, {700, 500}, {50, 500}
	};
	HBRUSH lightPurpleBrush = CreateSolidBrush(RGB(230, 210, 255));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, lightPurpleBrush);
	Polygon(hDC, backPoints, 4);

	// �ظ� (���� ����)
	POINT bottomPoints[] = {
		{100, 500}, {700, 500}, {750, 550}, {50, 550}
	};
	Polygon(hDC, bottomPoints, 4);

	// ���ʸ� (���� ����)
	POINT leftPoints[] = {
		{50, 100}, {100, 50}, {100, 500}, {50, 550}
	};
	HBRUSH darkPurpleBrush = CreateSolidBrush(RGB(170, 130, 255));
	SelectObject(hDC, darkPurpleBrush);
	Polygon(hDC, leftPoints, 4);

	// �����ʸ� (���� ����)
	POINT rightPoints[] = {
		{700, 50}, {750, 100}, {750, 550}, {700, 500}
	};
	Polygon(hDC, rightPoints, 4);
	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));  // �� ���⸦ 3���� ����
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	// �� �׸���
	MoveToEx(hDC, 100, 50, nullptr);
	LineTo(hDC, 700, 50);
	LineTo(hDC, 750, 100);
	LineTo(hDC, 50, 100);
	LineTo(hDC, 100, 50);

	MoveToEx(hDC, 50, 100, nullptr);
	LineTo(hDC, 50, 550);
	LineTo(hDC, 750, 550);
	LineTo(hDC, 750, 100);

	MoveToEx(hDC, 750, 550, nullptr);
	LineTo(hDC, 700, 500);
	LineTo(hDC, 700, 50);

	MoveToEx(hDC, 50, 550, nullptr);
	LineTo(hDC, 100, 500);
	LineTo(hDC, 100, 50);

	MoveToEx(hDC, 100, 500, nullptr);
	LineTo(hDC, 700, 500);

	// ���ҽ� ����
	SelectObject(hDC, hOldPen);
	SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(lightPurpleBrush);
	DeleteObject(darkPurpleBrush);
}

void CJWScene::Find_Proximate_Fruit()
{
	auto& fruitList = CObjectManager::Get_Instance()->Get_ObjList_ByID(OBJ_PLAYER);
	if (fruitList.size() == 1) return;
	CFruit* pNearFruit = nullptr; // ���� ����� ����
	CFruit* pDropFruit = nullptr; // ���� ����߸� ����

	float fNearDist = numeric_limits<float>::max();

	for (const auto& fruit : fruitList)
	{
		if (!fruit) continue;
		CFruit* pTmpFruit = dynamic_cast<CFruit*>(fruit);
		if (!pTmpFruit) continue;

		if (!pTmpFruit->Is_Dropped())
		{
			pDropFruit = pTmpFruit;
			break;  // ã������ �ٷ� ����������
		}
	}

	if (!pDropFruit) return;

	// ������ ���ϵ� �߿��� ���� ����� ���� ã��
	for (const auto& fruit : fruitList)
	{
		if (!fruit) continue;
		CFruit* pTmpFruit = dynamic_cast<CFruit*>(fruit);
		if (!pTmpFruit || !pTmpFruit->Is_Dropped()||pTmpFruit==pDropFruit) continue;

		D3DXVECTOR3 vDir = fruit->Get_Info().vPos - pDropFruit->Get_Info().vPos;
		float fDist = D3DXVec3Length(&vDir);
		if (fDist < fNearDist)
		{
			pNearFruit = pTmpFruit;
			fNearDist = fDist;
		}
	}

	if (!pNearFruit|| !pNearFruit->In_Box()) return;

	//float fLen = pNearFruit->Get_Info().vPos.y - (pNearFruit->Get_Radius() * pNearFruit->Get_Scale().y
	//	+pDropFruit->Get_Radius()*2* pDropFruit->Get_Scale().y);
	//m_fDropLenDebug = fLen;
	//if (fLen <= 70.f)  // y���� 70���� ������ ���� ���� ���
	//{
	//	m_bCanWarning = true;
	//}
	//else
	//{
	//	m_bCanWarning = false;
	//}
	D3DXVECTOR3 vDir = pNearFruit->Get_Info().vPos - pDropFruit->Get_Info().vPos;
	float fDist = D3DXVec3Length(&vDir); // �� �߽� �� �Ÿ�

	// 2. �� ���� ������ �� ���
	float fRadiusSum = (pNearFruit->Get_Radius() * pNearFruit->Get_Scale().y) +
		(pDropFruit->Get_Radius() * pDropFruit->Get_Scale().y);

	// 3. �浹 ���� ���� Ȯ��
	m_fDropLenDebug = fDist; // ������ �Ÿ� Ȯ��
	if (fDist <= fRadiusSum + 70.f) // �Ÿ� ���� + ���� �Ÿ�
	{
		m_bCanWarning = true; // �浹 ���
	}
	else
	{
		m_bCanWarning = false; // �浹 ����
	}

}

void CJWScene::Blink_Line()
{
	//static DWORD dwBlinkTime = GetTickCount64();
	//static bool bShow = true;

	//if (m_bCanWarning)
	//{
	//	// 250ms���� ������ ���� ��ȯ
	//	if (GetTickCount64() - dwBlinkTime >= 250)
	//	{
	//		bShow = !bShow;
	//		dwBlinkTime = GetTickCount64();
	//	}
	//}
	//else
	//{
	//	bShow = false;
	//	dwBlinkTime = GetTickCount64();
	//}

	// ��� ���¸� ��� ������ ����
}

void CJWScene::Update_Next_FruitInfo(const vector<D3DXVECTOR3>& vecRenderPoints, const COLORREF tColor, FRUIT_TYPE eType)
{
	m_tNextFruitInfo.vecRenderPoints = vecRenderPoints;
	m_tNextFruitInfo.tColor = tColor;
	m_tNextFruitInfo.eFruitType = eType;
}
