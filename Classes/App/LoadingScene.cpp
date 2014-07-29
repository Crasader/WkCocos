#include "WkCocosApp/LoadingScene.h"

#include "WkCocosApp/LoadingUI.h"

#include "WkCocosApp/ErrorUI.h"

#include "WkCocos/Download/Systems/ProgressUpdate.h"
#include "WkCocos/Preload/Systems/ProgressUpdate.h"

#include "ui/CocosGUI.h"

#include <iostream>
#include <numeric>

USING_NS_CC;

LoadingScene::LoadingScene() : Scene()
, m_loadDoneCB_called(false)
, m_loadMan_del_scheduled(false)
, m_loadDoneCB()
, m_downloadManager(nullptr)
, m_preloadManager(nullptr)
{
	m_downloadManager = new WkCocos::Download::Download(5,
		std::bind(&LoadingScene::progress_CB, this, std::placeholders::_1),
		std::bind(&LoadingScene::error_CB, this));
	m_preloadManager = new WkCocos::Preload::Preload(1,
		std::bind(&LoadingScene::progress_CB, this, std::placeholders::_1),
		std::bind(&LoadingScene::error_CB, this));
}

LoadingScene::~LoadingScene()
{
	if (m_downloadManager)
		delete m_downloadManager;
	if (m_preloadManager)
		delete m_preloadManager;
}

// on "init" you need to initialize your instance
bool LoadingScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//Load UI
	LoadingUI* loadui = new LoadingUI();
	loadui->getRoot()->setEnabled(true);
	loadui->getRoot()->setVisible(true);
	addChild(loadui->getRoot());
	m_ui[LoadingUI::id] = loadui;

	//Error UI
	ErrorUI* errorui = new ErrorUI();
	auto errorroot = errorui->getRoot();
	addChild(errorroot);
	errorroot->setEnabled(false);
	errorroot->setVisible(false);
	m_ui[ErrorUI::id] = errorui;

	errorui->setRefreshCallback([this, errorui](){
		
		m_downloadManager = new WkCocos::Download::Download(5,
			std::bind(&LoadingScene::progress_CB, this, std::placeholders::_1),
			std::bind(&LoadingScene::error_CB, this));

		m_preloadManager = new WkCocos::Preload::Preload(1,
			std::bind(&LoadingScene::progress_CB, this, std::placeholders::_1),
			std::bind(&LoadingScene::error_CB, this));

		m_downloadManager->start();
		m_preloadManager->start();
		m_preloadManager->setEventEmmiter(m_downloadManager->getEventManager());

		scheduleDLCCheck();

		errorui->deactivate();

	});

	errorui->setSkipCallback([this, errorui](){

		errorui->deactivate();

		m_loadDoneCB_called = true;
		m_loadDoneCB();

	});

	m_downloadManager->start();
	m_preloadManager->start();
	m_preloadManager->setEventEmmiter(m_downloadManager->getEventManager());


	return true;
}

void LoadingScene::onEnterTransitionDidFinish()
{
	//launching update method
	scheduleUpdate();
}

void LoadingScene::onExitTransitionDidStart()
{

}

void LoadingScene::addLoad(std::vector<std::string> respath)
{
	if (m_preloadManager)
		m_preloadManager->addDataLoad(respath);
}

void LoadingScene::setLoadDoneCallback(std::function<void()> cb)
{
	m_loadDoneCB = cb;
}

void LoadingScene::scheduleDLCCheck()
{
	if (m_downloadManager)
		m_downloadManager->addDataDownload("manifest.json");
}

void LoadingScene::update(float delta)
{
	if (m_loadMan_del_scheduled)
	{
		delete m_downloadManager;
		m_downloadManager = nullptr;
		delete m_preloadManager;
		m_preloadManager = nullptr;
		m_loadMan_del_scheduled = false;
	}

	//if callback was called, loading is finished.
	if (!m_loadDoneCB_called && m_downloadManager && m_preloadManager)
	{
		m_downloadManager->step(delta);
		m_preloadManager->step(delta);

		int downCurProgVal = m_downloadManager->getSystemManager()->system<WkCocos::Download::Systems::ProgressUpdate>()->curProgVal;
		int downTotProgVal = m_downloadManager->getSystemManager()->system<WkCocos::Download::Systems::ProgressUpdate>()->totalProgValMax;
		int preCurProgVal = m_preloadManager->getSystemManager()->system<WkCocos::Preload::Systems::ProgressUpdate>()->curProgVal;
		int preTotProgVal = m_preloadManager->getSystemManager()->system<WkCocos::Preload::Systems::ProgressUpdate>()->totalProgValMax;

		float pct = 1.0f - (float)(downCurProgVal + preCurProgVal) / (float)(downTotProgVal + preTotProgVal);

		LoadingUI* ui = getInterface<LoadingUI>(LoadingUI::id);
		if (ui) {
			ui::Widget* loadbarw = ui->getRoot()->getChildByName("LoadingBar");
			ui::LoadingBar* loadbar = dynamic_cast<ui::LoadingBar*>(loadbarw);
			if (loadbar)
				loadbar->setPercent(static_cast<int>(pct * 100));
		}

		if (pct >= 1.0f && !m_loadDoneCB_called) {
			FileUtils::getInstance()->purgeCachedEntries();
			m_loadDoneCB_called = true;
			m_loadDoneCB();
		}

	}
}

//expects pct in [0..1]
void LoadingScene::progress_CB(float pct)
{
	/*LoadingUI* ui = getInterface<LoadingUI>(LoadingUI::id);
	if (ui)
	{
		ui::Widget* loadbarw = ui->getRoot()->getChildByName("LoadingBar"); //loadbarpnl->getChildByName("LoadingBar");
		ui::LoadingBar* loadbar = dynamic_cast<ui::LoadingBar*>(loadbarw);

		if (loadbar)
			loadbar->setPercent(static_cast<int>(pct * 100));

	}

	if (pct >= 1.0f && !m_loadDoneCB_called)
	{
		FileUtils::getInstance()->purgeCachedEntries();
		//loading finished. lets move on.
		m_loadDoneCB_called = true;
		m_loadDoneCB();
	}*/
}

void LoadingScene::error_CB()
{

	//Error UI
	ErrorUI* errorui = getInterface<ErrorUI>(ErrorUI::id);
	errorui->activate();

	m_loadMan_del_scheduled = true;

}