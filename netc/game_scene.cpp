#include <array>

#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/utils.h>

#include "glbase.h"
#include "game_scene.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"

namespace ygopro
{

    GameScene gameScene;
	Random globalRandom;
	CommonConfig commonCfg;
    CommonConfig stringCfg;
    
    void GameScene::Init() {
        glGenBuffers(1, &index_buffer);
        glGenBuffers(1, &deck_buffer);
        glGenBuffers(1, &back_buffer);
        glGenBuffers(1, &misc_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * 4, nullptr, GL_DYNAMIC_DRAW);
        std::array<unsigned short, 256 * 6> index;
        for(int i = 0; i < 256; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 256 * 6, &index[0], GL_STATIC_DRAW);
    }
    
    void GameScene::Uninit() {
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &deck_buffer);
        glDeleteBuffers(1, &back_buffer);
        glDeleteBuffers(1, &misc_buffer);
    }
    
    void GameScene::UpdateScene() {
        UpdateBackGround();
        UpdateCardAll();
    }
    
    void GameScene::UpdateBackGround() {
        auto ti = imageMgr.GetTexture("bg");
        auto tex = imageMgr.GetTexInfo(0);
        std::array<glbase::VertexVCT, 4> verts;
        verts[0].vertex[0] = -1.0f;
        verts[0].vertex[1] = 1.0f;
        verts[0].texcoord[0] = (float)ti.x / tex.GetWidth();
        verts[0].texcoord[1] = (float)ti.y / tex.GetHeight();
        verts[1].vertex[0] = 1.0f;
        verts[1].vertex[1] = 1.0f;
        verts[1].texcoord[0] = (float)(ti.x + ti.w) / tex.GetWidth();
        verts[1].texcoord[1] = (float)ti.y / tex.GetHeight();
        verts[2].vertex[0] = -1.0f;
        verts[2].vertex[1] = -1.0f;
        verts[2].texcoord[0] = (float)ti.x / tex.GetWidth();
        verts[2].texcoord[1] = (float)(ti.y + ti.h) / tex.GetWidth();
        verts[3].vertex[0] = 1.0f;
        verts[3].vertex[1] = -1.0f;
        verts[3].texcoord[0] = (float)(ti.x + ti.w) / tex.GetWidth();
        verts[3].texcoord[1] = (float)(ti.y + ti.h) / tex.GetWidth();
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::VertexVCT) * verts.size(), &verts[0]);

    }
    
    void GameScene::UpdateCard(int pos, int index) {
        
    }
    
    void GameScene::UpdateCardAll() {
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz > deck_buffer_size) {
            if(deck_buffer_size == 0)
                deck_buffer_size = 128;
            while(deck_buffer_size < deck_sz)
                deck_buffer_size *= 1.5f;
        }
    }
    
    void GameScene::Draw() {
        imageMgr.BindTexture(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        // background
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glVertexPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(glbase::VertexVCT), (const GLvoid*)glbase::VertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), (const GLvoid*)glbase::VertexVCT::tex_offset);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        //
    }
    
    void GameScene::InitDraw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_INDEX_ARRAY);
        glLoadIdentity();
    }
  /*
    GameFrame::GameFrame(int sx, int sy) : wxFrame(nullptr, wxID_ANY, stringCfg["eui_msg_deck_title_new"], wxDefaultPosition, wxSize(sx, sy)) {
        
        wxMenuBar* menu_bar = new wxMenuBar;
        wxMenu* m_deck = new wxMenu();
        wxString stt = stringCfg["eui_menu_deck_load"];
        m_deck->Append(ID_DECK_LOAD, stt);
        m_deck->Append(ID_DECK_SAVE, stringCfg["eui_menu_deck_save"]);
        m_deck->Append(ID_DECK_SAVEAS, stringCfg["eui_menu_deck_saveas"]);
        m_deck->Append(ID_DECK_LOADSTR, stringCfg["eui_menu_deck_loadstr"]);
        m_deck->Append(ID_DECK_SAVESTR, stringCfg["eui_menu_deck_savestr"]);
        m_deck->Bind(wxEVT_MENU, &GameFrame::OnDeckLoad, this, ID_DECK_LOAD);
        m_deck->Bind(wxEVT_MENU, &GameFrame::OnDeckSave, this, ID_DECK_SAVE);
        m_deck->Bind(wxEVT_MENU, &GameFrame::OnDeckSaveAs, this, ID_DECK_SAVEAS);
        m_deck->Bind(wxEVT_MENU, &GameFrame::OnDeckLoadString, this, ID_DECK_LOADSTR);
        m_deck->Bind(wxEVT_MENU, &GameFrame::OnDeckSaveString, this, ID_DECK_SAVESTR);
        
        wxMenu* m_tool = new wxMenu();
        m_tool->Append(ID_TOOL_CLEAR, stringCfg["eui_menu_tool_clear"]);
        m_tool->Append(ID_TOOL_SORT, stringCfg["eui_menu_tool_sort"]);
        m_tool->Append(ID_TOOL_SHUFFLE, stringCfg["eui_menu_tool_shuffle"]);
        m_tool->Append(ID_TOOL_SCREENSHOT, stringCfg["eui_menu_tool_screenshot"]);
        m_tool->Append(ID_TOOL_SCREENSHOT_SV, stringCfg["eui_menu_tool_saveshot"]);
        m_tool->Append(ID_TOOL_SEARCH, stringCfg["eui_menu_tool_search"]);
        m_tool->Append(ID_TOOL_BROWSER, stringCfg["eui_menu_tool_browser"]);
        m_tool->AppendSeparator();
        m_tool->Append(ID_TOOL_SHOWEXCLUSIVE, stringCfg["eui_menu_tool_show_exclusive"], wxEmptyString, true);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolClear, this, ID_TOOL_CLEAR);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolSort, this, ID_TOOL_SORT);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolShuffle, this, ID_TOOL_SHUFFLE);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolScreenshot, this, ID_TOOL_SCREENSHOT);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolScreenshotSV, this, ID_TOOL_SCREENSHOT_SV);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolSearch, this, ID_TOOL_SEARCH);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolOpenBrowser, this, ID_TOOL_BROWSER);
        m_tool->Bind(wxEVT_MENU, &GameFrame::OnToolSwitchExclusive, this, ID_TOOL_SHOWEXCLUSIVE);

        wxMenu* m_limit = new wxMenu();
        auto& lrs = limitRegulationMgr.GetLimitRegulations();
        for(unsigned int i = 0; i < lrs.size(); ++i) {
            if(i < 3) {
                wxString extra = wxString::Format(wxT("\tALT+%d"), i + 1);
                m_limit->Append(ID_REGULATION + i, lrs[i].name + extra, wxEmptyString, true);
            } else
                m_limit->Append(ID_REGULATION + i, lrs[i].name, wxEmptyString, true);
            m_limit->Bind(wxEVT_MENU, &GameFrame::OnRegulationChange, this, ID_REGULATION + i);
        }
        if(lrs.size() > 0)
            m_limit->GetMenuItems()[0]->Check(true);
        wxMenu* m_vlist = new wxMenu;
        m_vlist->Append(ID_REG_VIEW1, stringCfg["eui_list_forbidden"]);
        m_vlist->Append(ID_REG_VIEW2, stringCfg["eui_list_limit"]);
        m_vlist->Append(ID_REG_VIEW3, stringCfg["eui_list_semilimit"]);
        m_vlist->Bind(wxEVT_MENU, &GameFrame::OnToolViewRegulation, this);
        m_limit->AppendSeparator();
        m_limit->AppendSubMenu(m_vlist, stringCfg["eui_list_view"]);
        
        menu_bar->Append(m_deck, stringCfg["eui_menu_deck"]);
        menu_bar->Append(m_tool, stringCfg["eui_menu_tools"]);
        menu_bar->Append(m_limit, stringCfg["eui_menu_reg"]);
        SetMenuBar(menu_bar);

        card_image = new wxStaticBitmap(this, wxID_ANY, wxBitmap(177, 254));
        card_info = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(240, 10000), wxRE_READONLY | wxRE_MULTILINE);
        card_info->Bind(wxEVT_TEXT_URL, &GameFrame::OnUrlClicked, this, wxID_ANY);
        card_info->GetCaret()->Hide();
        int wx_gl_attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
        game_canvas = new wxGameCanvas(this, wxID_ANY, wx_gl_attribs);

        wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
        sz->AddSpacer(5);
        sz->Add(card_image, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->AddSpacer(5);
        sz->Add(card_info, 1, wxEXPAND | wxALL, 5);
        wxBoxSizer *sz2 = new wxBoxSizer(wxHORIZONTAL);
        //sz2->Add(sz);
        sz2->Add(game_canvas, 1, wxEXPAND | wxALL);
        SetSizer(sz2);
        
        mainFrame = this;
        SetCardInfo(0);
        game_canvas->SetFocus();

        TextureInfo& st = imageMgr.textures["star"];
        if(st.src) {
            int x1 = st.src->t_width * st.lx;
            int y1 = st.src->t_height * st.ly;
            int x2 = st.src->t_width * st.rx;
            int y2 = st.src->t_height * st.ry;
            star_img = st.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1));
        }
    }

    GameFrame::~GameFrame() {
        if(filterFrame)
            delete filterFrame;
    }

    void GameFrame::SetCardInfo(unsigned int code) {
        static unsigned int current_code = 0;
        if(current_code == code)
            return;
        current_code = code;
        if(code == 0) {
            TextureInfo& sv = imageMgr.textures["sleeve1"];
            if(sv.src == nullptr)
                return;
            card_info->Clear();
            int x1 = sv.src->t_width * sv.lx;
            int y1 = sv.src->t_height * sv.ly;
            int x2 = sv.src->t_width * sv.rx;
            int y2 = sv.src->t_height * sv.ry;
            card_image->SetBitmap(wxBitmap(sv.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1))));
        } else {
            wxImage card_img;
            wxString file = wxString::Format("%s/%d.jpg", ((const std::string&)commonCfg["image_path"]).c_str(), code);
            if(wxFileExists(file) && card_img.LoadFile(file)) {
                card_image->SetBitmap(wxBitmap(card_img));
            } else {
                TextureInfo& sv = imageMgr.textures["unknown"];
                if(sv.src == nullptr)
                    return;
                int x1 = sv.src->t_width * sv.lx;
                int y1 = sv.src->t_height * sv.ly;
                int x2 = sv.src->t_width * sv.rx;
                int y2 = sv.src->t_height * sv.ry;
                card_image->SetBitmap(wxBitmap(sv.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1))));
            }
            card_info->Clear();
            CardData* cd = dataMgr[code];
            if(cd == nullptr)
                return;
            wxString card_name = cd->name;
            wxRichTextAttr attr;
            card_info->SetDefaultStyle(attr);
            card_info->BeginFontSize(16);
            card_info->BeginBold();
            card_info->WriteText(card_name);
            card_info->EndBold();
            card_info->EndFontSize();
            card_info->Newline();
            if(cd->type & 0x1) {
                for(unsigned int i = 0; i < cd->level; ++i)
                    card_info->WriteImage(star_img);
                card_info->Newline();
                card_info->BeginTextColour(wxColour(180, 140, 40));
                wxString infostr = wxT("[") + dataMgr.GetTypeString(cd->type) + wxT("] ") + dataMgr.GetAttributeString(cd->attribute) + wxT("/") + dataMgr.GetRaceString(cd->race);
                card_info->WriteText(infostr);
                card_info->EndTextColour();
                card_info->Newline();
                card_info->BeginTextColour(wxColour(64, 64, 64));
                if(cd->attack >= 0 && cd->defence >= 0)
                    card_info->WriteText(wxString::Format(wxT("ATK:%d / DEF:%d"), cd->attack, cd->defence));
                else if(cd->attack >= 0)
                    card_info->WriteText(wxString::Format(wxT("ATK:%d / DEF:????"), cd->attack));
                else if(cd->defence >= 0)
                    card_info->WriteText(wxString::Format(wxT("ATK:???? / DEF:%d"), cd->defence));
                else
                    card_info->WriteText(wxString::Format(wxT("ATK:???? / DEF:????")));
                card_info->EndTextColour();
                card_info->Newline();
            } else {
                if(cd->type & 0x2)
                    card_info->BeginTextColour(wxColour(0, 192, 128));
                else
                    card_info->BeginTextColour(wxColour(250, 32, 192));
                wxString infostr = wxT("[") + dataMgr.GetTypeString(cd->type) + wxT("] ");
                card_info->WriteText(infostr);
                card_info->EndTextColour();
                card_info->Newline();
            }
            card_info->Newline();
            wxString card_text = cd->texts;
            wxString parse_begin = stringCfg["parse_begin"];
            wxString parse_end = stringCfg["parse_end"];
            size_t start = 0, begin = 0, end = 0, length = card_text.length();
            while(start < length) {
                begin = card_text.find_first_of(parse_begin, start);
                if(begin == -1) {
                    card_info->WriteText(card_text.substr(start));
                    break;
                }
                end = card_text.find_first_of(parse_end, begin + 1);
                while(end != -1 && card_text[end - 1] == wxT('\\'))
                    end = card_text.find_first_of(parse_end, end + 1);
                if(end == -1) {
                    card_info->WriteText(card_text.substr(start));
                    break;
                }
                if(begin > start)
                    card_info->WriteText(card_text.substr(start, begin - start));
                card_info->BeginTextColour(wxColour(0, 0, 255));
                card_info->BeginURL(card_text.substr(begin + 1, end - begin - 1));
                card_info->WriteText(card_text.substr(begin, end - begin + 1));
                card_info->EndURL();
                card_info->EndTextColour();
                start = end + 1;
            }
            card_info->Newline();
            if((cd->type & 0x4000) == 0) {
                card_info->Newline();
                card_info->WriteText(stringCfg["eui_infocode"]);
                unsigned int code = cd->code;
                if(cd->alias && ((cd->alias > code && cd->alias - code < 10) || (cd->alias < code && code - cd->alias < 10)))
                    code = cd->alias;
                card_info->WriteText(wxString::Format(wxT("%08d"), code));
            }
            unsigned long long setcode = cd->setcode;
            if(cd->alias) {
                CardData* alias = dataMgr[cd->alias];
                if(alias)
                    setcode = alias->setcode;
            }
            if(setcode) {
                card_info->Newline();
                card_info->WriteText(stringCfg["eui_category"]);
                wxString setname = dataMgr.GetSetCode(setcode & 0xffff);
                card_info->BeginTextColour(wxColour(0, 0, 255));
                if(!setname.IsEmpty()) {
                    card_info->BeginURL(setname);
                    card_info->WriteText(wxT("[") + setname + wxT("] "));
                    card_info->EndURL();
                }
                setname = dataMgr.GetSetCode((setcode >> 16) & 0xffff);
                if(!setname.IsEmpty()) {
                    card_info->BeginURL(setname);
                    card_info->WriteText(wxT("[") + setname + wxT("]"));
                    card_info->EndURL();
                }
                setname = dataMgr.GetSetCode((setcode >> 32) & 0xffff);
                if(!setname.IsEmpty()) {
                    card_info->BeginURL(setname);
                    card_info->WriteText(wxT("[") + setname + wxT("]"));
                    card_info->EndURL();
                }
                card_info->EndTextColour();
            }
        }
    }
    
    void GameFrame::AddCard(unsigned int code, unsigned int pos) {
        game_canvas->GetDeck().InsertCard(code, pos, -1, false, true);
        StopViewRegulation();
        game_canvas->Refresh();
    }
    
    void GameFrame::OnDeckLoad(wxCommandEvent& evt) {
        wxFileDialog fd(this, stringCfg["eui_msg_deck_load"], wxEmptyString, wxEmptyString, stringCfg["eui_msg_deck_filter"], wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if(fd.ShowModal() != wxID_OK)
            return;
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(fd.GetPath())) {
            game_canvas->ClearDeck();
            game_canvas->GetDeck() = tempdeck;
            current_file = fd.GetPath();
            wxString title = stringCfg["eui_msg_deck_title"];
            title.Replace(wxT("{deck}"), current_file);
            SetTitle(title);
            game_canvas->Refresh();
        } else {
            wxMessageDialog(this, stringCfg["eui_msg_deck_load_error"]);
        }
    }

    void GameFrame::OnDeckSave(wxCommandEvent& evt) {
        if(current_file.IsEmpty())
            OnDeckSaveAs(evt);
        else
            game_canvas->GetDeck().SaveToFile(current_file);
    }

    void GameFrame::OnDeckSaveAs(wxCommandEvent& evt) {
        wxFileDialog fd(this, stringCfg["eui_msg_deck_save"], wxEmptyString, wxEmptyString, stringCfg["eui_msg_deck_filter"], wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if(fd.ShowModal() != wxID_OK)
            return;
        game_canvas->GetDeck().SaveToFile(fd.GetPath());
        current_file = fd.GetPath();
        wxString title = stringCfg["eui_msg_deck_title"];
        title.Replace(wxT("{deck}"), current_file);
        SetTitle(title);
    }

    void GameFrame::OnDeckLoadString(wxCommandEvent& evt) {
        wxTextDataObject tdo;
        wxTheClipboard->Open();
        wxTheClipboard->GetData(tdo);
        wxTheClipboard->Close();
        wxString deck_string = tdo.GetText();
        DeckData tempdeck;
        if(deck_string.Left(6) == wxT("ydk://") && tempdeck.LoadFromString(deck_string.Right(deck_string.Length() - 6))) {
            game_canvas->ClearDeck();
            game_canvas->GetDeck() = tempdeck;
            game_canvas->Refresh();
        } else {
            wxMessageDialog(this, stringCfg["eui_msg_str_load_error"], wxEmptyString, wxICON_ERROR).ShowModal();
        }
    }

    void GameFrame::OnDeckSaveString(wxCommandEvent& evt) {
        wxString deck_string = game_canvas->GetDeck().SaveToString();
        deck_string = wxT("ydk://") + deck_string;
        wxTheClipboard->Open();
        wxTheClipboard->SetData(new wxTextDataObject(deck_string));
        wxTheClipboard->Close();
        game_canvas->Refresh();
        wxMessageDialog(this, stringCfg["eui_msg_str_save"], wxEmptyString, wxICON_INFORMATION).ShowModal();
    }

    void GameFrame::OnToolClear(wxCommandEvent& evt) {
        game_canvas->ClearDeck();
        game_canvas->Refresh();
    }

    void GameFrame::OnToolSort(wxCommandEvent& evt) {
        game_canvas->GetDeck().Sort();
        game_canvas->Refresh();
    }

    void GameFrame::OnToolShuffle(wxCommandEvent& evt) {
        game_canvas->GetDeck().Shuffle();
        game_canvas->Refresh();
    }

    void GameFrame::OnToolScreenshot(wxCommandEvent& evt) {
        game_canvas->SaveScreenshot(wxEmptyString, true);
        wxMessageDialog(this, stringCfg["eui_msg_screen_copy"], wxEmptyString, wxICON_INFORMATION).ShowModal();
    }

    void GameFrame::OnToolScreenshotSV(wxCommandEvent& evt) {
        time_t t = time(0);
        wxString path = commonCfg["screenshot_path"];
        if(*path.rbegin() != wxT('/'))
            path.Append(wxT('/'));
        if(!wxFileName::DirExists(path) && !wxFileName::Mkdir(path)) {
            wxString err = stringCfg["eui_msg_screen_err"];
            err.Replace(wxT("{path}"), path);
            wxMessageDialog(this, err, wxEmptyString, wxICON_ERROR).ShowModal();
            return;
        }
        path += wxString::Format(wxT("%d.png"), (int)t);
        game_canvas->SaveScreenshot(path);
        wxString savemsg = stringCfg["eui_msg_screen_save"];
        savemsg.Replace(wxT("{path}"), path);
        wxMessageDialog(this, savemsg, wxEmptyString, wxICON_INFORMATION).ShowModal();
    }

    void GameFrame::OnToolSearch(wxCommandEvent& evt) {
        if(filterFrame)
            filterFrame->Raise();
        else {
            filterFrame = new FilterFrame();
            filterFrame->Center();
            filterFrame->Show();
        }
    }

    void GameFrame::OnToolOpenBrowser(wxCommandEvent& evt) {
        wxString neturl = commonCfg["deck_neturl"];
        wxString deck_string = game_canvas->GetDeck().SaveToString();
        neturl.Replace("{amp}", wxT("&"));
        neturl.Replace("{deck}", deck_string);
        if(current_file.IsEmpty())
            neturl.Replace("{name}", wxEmptyString);
        else {
            wxFileName fn(current_file);
            neturl.Replace("{name}", fn.GetName());
        }
        game_canvas->Refresh();
        wxLaunchDefaultBrowser(neturl);
    }

    void GameFrame::OnToolSwitchExclusive(wxCommandEvent& evt) {
        game_canvas->SwitchShowExclusive();
        Refresh();
    }

    void GameFrame::OnToolViewRegulation(wxCommandEvent& evt) {
        int id = evt.GetId() - ID_REG_VIEW1;
        view_regulation = id + 1;
        limitRegulationMgr.LoadCurrentListToDeck(game_canvas->GetDeck(), id);
        game_canvas->Refresh();
    }
    
    void GameFrame::OnRegulationChange(wxCommandEvent& evt) {
        auto& regs = GetMenuBar()->GetMenu(2)->GetMenuItems();
        if(regs.GetCount() == 0)
            return;
        bool change = true;
        int id = evt.GetId() - ID_REGULATION;
        for(size_t i = 0; i < regs.GetCount(); ++i) {
            auto mi = regs[i];
            if(mi->IsCheck() && id == i)
                change = false;
            mi->Check(id == i);
        }
        limitRegulationMgr.SetLimitRegulation(id);
        if(view_regulation)
            limitRegulationMgr.LoadCurrentListToDeck(game_canvas->GetDeck(), view_regulation - 1);
        limitRegulationMgr.GetDeckCardLimitCount(game_canvas->GetDeck());
        game_canvas->Refresh();
    }
    
    void GameFrame::OnUrlClicked(wxTextUrlEvent& evt) {
        wxString url = evt.GetString();
        if(filterFrame)
            filterFrame->Raise();
        else {
            filterFrame = new FilterFrame();
            filterFrame->Center();
            filterFrame->Show();
        }
        filterFrame->FilterCards(url);
    }
*/
}
