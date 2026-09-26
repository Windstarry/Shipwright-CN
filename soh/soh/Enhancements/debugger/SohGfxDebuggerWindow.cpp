#include "SohGfxDebuggerWindow.h"
#include "soh/OTRGlobals.h"

void SohGfxDebuggerWindow::InitElement() {
    GfxDebuggerWindow::InitElement();
}

void SohGfxDebuggerWindow::UpdateElement() {
    GfxDebuggerWindow::UpdateElement();
}

void SohGfxDebuggerWindow::DrawElement() {
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
    ImGui::PushFont(OTRGlobals::Instance->GetLocalizableFont(OTRGlobals::Instance->fontMonoLarger, OTRGlobals::Instance->fontStandardLarger));
    GfxDebuggerWindow::DrawElement();
    ImGui::PopFont();
    ImGui::EndDisabled();
}
