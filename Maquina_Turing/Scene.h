#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED
#include "stdafx.h"
class Scene{
public:
	Scene();
	~Scene(){}
	int cursorPos;
	std::shared_ptr<TextLabel>lbLang,lbInput,lbResultTitle,lbResultStatus,lbMarkerPos;
	std::shared_ptr<ImageView>imgFail,imgSuccess;
	std::shared_ptr<ImageView>marker;
	std::vector<std::shared_ptr<TextLabel>> tape;
	std::vector<std::shared_ptr<ImageView>> tapeSquares;
	std::shared_ptr<ImageView>selection;
	std::shared_ptr<Button>btnStart;
	std::shared_ptr<TextBox>tfInput;
	void recreateResources(std::shared_ptr<D2DHandle>&d2d,GameSettings settings);
};
#endif