#pragma once
#include <vector>
struct Rect {
    int x = 0, y = 0;
    int width = 0, height = 0;
    Rect() = default;
    Rect(int w, int h) : width(w), height(h), x(0), y(0) {}
    Rect(int xx, int yy, int w, int h) : width(w), height(h), x(xx), y(yy) {}
};

class RectPacker
{
public:
    RectPacker(int w, int h) {
        init(w, h);
    }
    ~RectPacker(){
        if(root){
            delete root;
            root = nullptr;
        }
    }

    void init(int w, int h){
        if(root){
            delete root;
            root = nullptr;
        }
        root = new Node;
        memset(root, 0, sizeof(Node));
        root->rect.width = w;
        root->rect.height = h;
    }
    inline void setInterval(int interval) { this->interval = interval; }
    Rect insert(int w, int h, bool* ok = 0) {
        Rect r;
        const bool inserted = insertSize(root, w, h, r);
        if (ok)
            *ok = inserted;
        return r;
    }
private:
    struct Node
    {
        Node* child[2];
        Rect rect;
        bool taken = false;
        Node(){
            child[0] = nullptr; 
            child[1] = nullptr;
        }
        ~Node(){
            if(child[0])
                delete child[0];
            if(child[1])
                delete child[1];
            child[0] = nullptr; 
            child[1] = nullptr;
        }
    };

    bool insertSize(Node* node, int w, int h, Rect& r) {
        int tw = w + interval + interval;
        int th = h + interval + interval;
        if (node->child[0]) {
            assert(node->child[1]);
            if (insertSize(node->child[0], w, h, r))
                return true;
            return insertSize(node->child[1], w, h, r);
        }
        if (node->taken)
            return false;
        if (node->rect.width < tw || node->rect.height < th)
            return false;
        if (tw == node->rect.width && th == node->rect.height) {
            node->taken = true;
            r = { node->rect.x + interval, node->rect.y+interval, w, h};
            return true;
        }
        node->child[0] = new Node;
        memset(node->child[0], 0, sizeof(Node));
        node->child[1] = new Node;
        memset(node->child[1], 0, sizeof(Node));

        int dw = node->rect.width - tw;
        int dh = node->rect.height - th;
        assert(dw >= 0 && dh >= 0);
        if (dw > dh) {
            node->child[0]->rect = Rect(node->rect.x, node->rect.y, tw, node->rect.height);
            node->child[1]->rect = Rect(node->rect.x + tw, node->rect.y, node->rect.width - tw, node->rect.height);
        } else {
            node->child[0]->rect = Rect(node->rect.x, node->rect.y, node->rect.width, th);
            node->child[1]->rect = Rect(node->rect.x, node->rect.y + th, node->rect.width, node->rect.height - th);
        }
        return insertSize(node->child[0], w, h, r);
    }

    int interval = 0;
    Node* root = nullptr;
};


#ifdef TEST_RECT_PACKER
struct RectPackerTest {
    std::vector<Rect> rects;
    bool randomize(RectPacker & pack)
    {
        srandom(time(0));
        enum { RectCount = 600,
               MinWidth = 10, MaxWidth = 50,
               MinHeight = 20, MaxHeight = 80 };
        bool ok;
        for (int i = 0; i < RectCount; ++i) {
            const int w = std::max<int>(random() % MaxWidth, MinWidth);
            const int h = std::max<int>(random() % MaxHeight, MinHeight);
            Rect r = pack.insert(w, h, &ok);
            if (!ok) {
                printf("Unable to fit size %dx%d\n", w, h);
                return false;
            }
            assert(r.width == w && r.height == h);
            rects.push_back(r);
        }
        return true;
    }
    bool resize(int w, int h){
        RectPacker pack(w, h);
        pack.setInterval(5);
        if(this->rects.empty()){
            return randomize(pack);
        }
        else {
            std::vector<Rect> temp_rects;
            for(auto rc : rects){
                bool bok = false;
                Rect temp_rc = pack.insert(rc.width, rc.height, &bok);
                if (!bok) {
                    printf("Unable to fit size %dx%d\n", rc.width, rc.height);
                    return false;
                }
                assert(temp_rc.width == rc.width && temp_rc.height == rc.height);
                temp_rects.push_back(temp_rc);
            }
            this->rects = temp_rects;
            return true;
        }
    }
};
#endif