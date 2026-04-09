#ifndef _LABEL_2D_RENDER_H_
#define _LABEL_2D_RENDER_H_

class LabelComponent;

class LabelRender
{
private:
    LabelComponent *component;
    int countValidChar();
    int validCharNum = 0;
    bool _drity = 0;
public:
    LabelRender(LabelComponent*);

    inline void setDirty(){ _drity = true; };

    const int& getValidCharNum(){ return validCharNum; };

    /**
     * 执行文字布局、渲染的入口
     */
    void render();
};

#endif