// $Id: ctrlComboBox.cpp 9357 2014-04-25 15:35:25Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "Loader.h"

#include "ctrlComboBox.h"

#include "ctrlButton.h"
#include "ctrlList.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ctrlComboBox.
 *
 *  @author OLiver
 */
ctrlComboBox::ctrlComboBox(Window* parent,
                           unsigned int id,
                           unsigned short x,
                           unsigned short y,
                           unsigned short width,
                           unsigned short height,
                           TextureColor tc,
                           glArchivItem_Font* font,
                           unsigned short max_list_height,
                           bool readonly)
    : Window(x, y, id, parent, width, height),
      tc(tc), font(font), max_list_height(max_list_height), readonly(readonly), selection(0), last_show(false)
{
    ctrlList* liste = AddList(0, 0, height, width, 4, tc, font);

    // Liste am Anfang nicht anzeigen
    liste->SetVisible(false);

    if(!readonly)
        AddImageButton(1, width - height, 0, height, height, tc, LOADER.GetImageN("io", 34));

    Resize_(width, height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Gr��e ver�ndern
 *
 *  @author Divan
 *  @author OLiver
 */
void ctrlComboBox::Resize_(unsigned short width, unsigned short height)
{
    ctrlButton* button = GetCtrl<ctrlButton>(1);
    if(button)
    {
        button->Move(width - height, 0);
        button->Resize(height, height);
    }

    ctrlList* list = GetCtrl<ctrlList>(0);

    unsigned short list_height = 4;

    // Langsam die H�he der maximalen ann�hern
    for(unsigned int i = 0; i < list->GetLineCount(); ++i)
    {
        // zu gro�e geworden?
        list_height += font->getHeight();

        if(list_height > (scale ? ScaleY(max_list_height) : max_list_height))
        {
            // kann nicht mal ein Item aufnehmen, dann raus
            if(i == 0)
                return;

            // H�he um eins erniedrigen, damits wieder kleiner ist als die maximale
            list_height -= font->getHeight();;

            break;
        }
    }

    list->Move(0, height);
    list->Resize(width, list_height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void ctrlComboBox::Msg_PaintAfter()
{
    // Liste erst jetzt malen, damit sie den Rest �berdeckt
    GetCtrl<ctrlList>(0)->Draw();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
bool ctrlComboBox::Msg_MouseMove(const MouseCoords& mc)
{
    // F�r Button und Liste weiterleiten
    return RelayMouseMessage(&Window::Msg_MouseMove, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
bool ctrlComboBox::Msg_LeftDown(const MouseCoords& mc)
{
    ctrlList* list = GetCtrl<ctrlList>(0);

    // Irgendwo anders hingeklickt --> Liste ausblenden
    if(!readonly && !Coll(mc.x, mc.y, GetX(), GetY(), width, height + list->GetHeight()))
    {
        // Liste wieder ausblenden
        ShowList(false);
        return false;
    }

    if(!readonly && Coll(mc.x, mc.y, GetX(), GetY(), width, height))
    {
        // Liste wieder ein/ausblenden
        ShowList(!list->GetVisible());
        return true;
    }

    // F�r Button und Liste weiterleiten
    return RelayMouseMessage(&Window::Msg_LeftDown, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
bool ctrlComboBox::Msg_LeftUp(const MouseCoords& mc)
{
    // F�r Button und Liste weiterleiten
    return RelayMouseMessage(&Window::Msg_LeftUp, mc);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author Divan
 */
bool ctrlComboBox::Msg_RightDown(const MouseCoords& mc)
{
    ctrlList* list = GetCtrl<ctrlList>(0);

    // F�r Button und Liste weiterleiten (und danach erst schlie�en)
    bool ret = RelayMouseMessage(&Window::Msg_RightDown, mc);

    // Clicked on list -> close it
    if(!readonly && Coll(mc.x, mc.y, GetX(), GetY() + height, width, height + list->GetHeight()))
    {
        // Liste wieder ausblenden
        ShowList(false);
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author Divan
 */
bool ctrlComboBox::Msg_WheelUp(const MouseCoords& mc)
{
    ctrlList* list = GetCtrl<ctrlList>(0);

    if(!readonly && Coll(mc.x, mc.y, GetX(), GetY() + height, width, height + list->GetHeight()) && list->GetVisible())
    {
        // Scrolled in opened list ->
        return RelayMouseMessage(&Window::Msg_WheelUp, mc);
    }

    if(!readonly && Coll(mc.x, mc.y, GetX(), GetY(), width, height))
    {
        // Scrolled without list opened
        if (list->GetSelection() > 0)
            Msg_ListSelectItem(GetID(), list->GetSelection() - 1);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author Divan
 */
bool ctrlComboBox::Msg_WheelDown(const MouseCoords& mc)
{
    ctrlList* list = GetCtrl<ctrlList>(0);

    if(!readonly && Coll(mc.x, mc.y, GetX(), GetY() + height, width, height + list->GetHeight()) && list->GetVisible())
    {
        // Scrolled in opened list ->
        return RelayMouseMessage(&Window::Msg_WheelDown, mc);
    }

    if(!readonly && Coll(mc.x, mc.y, GetX(), GetY(), width, height))
    {
        // Scrolled without list opened
        if (list->GetSelection() < list->GetLineCount() - 1)
            Msg_ListSelectItem(GetID(), list->GetSelection() + 1);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *  @author OLiver
 */
void ctrlComboBox::Msg_ListSelectItem(const unsigned int ctrl_id, const unsigned short selection)
{
    // Liste wieder ausblenden
    ShowList(false);

    ctrlList* list = GetCtrl<ctrlList>(0);

    // ist in der Liste �berhaupt was drin?
    if(selection != this->selection && list->GetLineCount() > 0)
    {
        this->selection = selection;

        // Nachricht an �bergeordnetes Fenster verschicken
        parent->Msg_ComboSelectItem(GetID(), selection);
    }
}


///////////////////////////////////////////////////////////////////////////////
/**
 *  f�gt einen String zur Liste hinzu.
 *
 *  @author OLiver
 */
void ctrlComboBox::AddString(const std::string& text)
{
    GetCtrl<ctrlList>(0)->AddString(text);
    Resize_(width, height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  l�scht alle Items der Liste.
 *
 *  @author OLiver
 */
void ctrlComboBox::DeleteAllItems()
{
    GetCtrl<ctrlList>(0)->DeleteAllItems();
    Resize_(width, height);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  w�hlt ein Item aus
 *
 *  @author OLiver
 */
void ctrlComboBox::SetSelection(unsigned short selection)
{
    this->selection = selection;
    GetCtrl<ctrlList>(0)->SetSelection(selection);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  zeichnet das Fenster.
 *
 *  @author OLiver
 */
bool ctrlComboBox::Draw_(void)
{
    ctrlList* liste = GetCtrl<ctrlList>(0);

    // Box
    Draw3D(GetX(), GetY(), width, height, tc, 2);

    // Namen des selektierten Strings in der Box anzeigen
    if(liste->GetLineCount() > 0)
        font->Draw(GetX() + 2, GetY() + height / 2, liste->GetSelItemText(), glArchivItem_Font::DF_VCENTER, COLOR_YELLOW, 0, width - 2 - height, "");

    // Male restliche Controls per Hand, denn ein einfaches DrawControls() w�rde
    // auch die Liste malen, die bei Msg_PaintAfter() sowieso gemalt wird.
    ctrlImageButton* button = GetCtrl<ctrlImageButton>(1);
    if(button)
        button->Draw();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  blendet die Liste ein oder aus.
 *
 *  @author OLiver
 */
void ctrlComboBox::ShowList(bool show)
{
    if(last_show == show)
        return;

    last_show = show;

    ctrlList* liste = GetCtrl<ctrlList>(0);

    // Liste entsprechend
    liste->SetVisible(show);

    // Pfeilbutton entsprechend
    GetCtrl<ctrlButton>(1)->SetCheck(show);

    // Region sperren f�r die Liste, oder freigeben
    if(show)
    {
        Rect list_region =
        {
            liste->GetX(),
            liste->GetY(),
            static_cast<unsigned short>(liste->GetX() + width),
            static_cast<unsigned short>(liste->GetY() + liste->GetHeight())
        };

        parent->LockRegion(this, list_region);
    }
    else
    {
        parent->FreeRegion(this);
    }

    LOADER.GetSoundN("sound", 113)->Play(255, false);
}
