#pragma once
#include "../includes/json/json.hpp"

namespace R3nzSkinInjector {

using json = nlohmann::json;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;
using namespace System::IO;

static auto btnState{false};
static auto gameState{false};
static auto cheatState{false};
static auto clientState{false};

public
ref class R3nzUI : public System::Windows::Forms::Form {
 public:
  R3nzUI(void) {
    InitializeComponent();
    loadSettings();
    if (autoInjectMenuItem->Checked) {
      injectButton_Click(nullptr, nullptr);
    }
  }

 public:
  void updateScreen() {
    while (true) {
      if (clientState) {
        this->clientStatusLabel->Text      = L"Found";
        this->clientStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 252, 220, 107);
      } else {
        this->clientStatusLabel->Text      = L"Not Found";
        this->clientStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 245, 8, 83);
      }

      if (gameState) {
        this->gameStatusLabel->Text      = L"Found";
        this->gameStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 252, 220, 107);
        if (cheatState) {
          this->r3nzSkinStatusLabel->Text      = L"Injected";
          this->r3nzSkinStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 252, 220, 107);
        } else {
          this->r3nzSkinStatusLabel->Text      = L"Not Injected";
          this->r3nzSkinStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 245, 8, 83);
        }
      } else {
        this->gameStatusLabel->Text          = L"Not Found";
        this->gameStatusLabel->ForeColor     = System::Drawing::Color::FromArgb(255, 245, 8, 83);
        this->r3nzSkinStatusLabel->Text      = L"Not Injected";
        this->r3nzSkinStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 245, 8, 83);
      }
      Thread::Sleep(1000);
    }
  }

  void loadSettings() {
    auto appDataPath      = Environment::GetFolderPath(Environment::SpecialFolder::MyDocuments);
    auto settingsFolder   = Path::Combine(appDataPath, L"R3nzSkin");
    auto settingsFilePath = Path::Combine(settingsFolder, L"R3nzSkinInjector");

    if (!Directory::Exists(settingsFolder)) {
      Directory::CreateDirectory(settingsFolder);
    }

    if (!File::Exists(settingsFilePath)) {
      return;
    }

    auto lines = File::ReadAllLines(settingsFilePath);
    if (lines->Length > 0) {
      bool oldHideToTray;
      if (System::Boolean::TryParse(lines[0], oldHideToTray)) {
        this->trayHideMenuItem->Checked = oldHideToTray;
        saveSettings();
        return;
      }
    }

    std::string settingsValue = msclr::interop::marshal_as<std::string>(File::ReadAllText(settingsFilePath));
    json jsonSettings         = json::parse(settingsValue);

    if (jsonSettings.contains("hideToTray")) {
      this->trayHideMenuItem->Checked = jsonSettings["hideToTray"];
    }

    if (jsonSettings.contains("autoInject")) {
      this->autoInjectMenuItem->Checked = jsonSettings["autoInject"];
    }
  }

  void saveSettings() {
    auto appDataPath      = Environment::GetFolderPath(Environment::SpecialFolder::MyDocuments);
    auto settingsFolder   = Path::Combine(appDataPath, L"R3nzSkin");
    auto settingsFilePath = Path::Combine(settingsFolder, L"R3nzSkinInjector");

    if (!Directory::Exists(settingsFolder)) {
      Directory::CreateDirectory(settingsFolder);
    }

    json jsonSettings;
    jsonSettings["hideToTray"] = this->trayHideMenuItem->Checked;
    jsonSettings["autoInject"] = this->autoInjectMenuItem->Checked;

    std::string jsonString = jsonSettings.dump();

    File::WriteAllText(settingsFilePath, gcnew String(jsonString.c_str()));
  }

 protected:
  ~R3nzUI() {
    if (components) {
      delete components;
    }
  }

 private:
  System::Windows::Forms::Button ^ injectButton;

 private:
  System::Windows::Forms::Label ^ injectorStatusLabel;

 private:
  System::Windows::Forms::Label ^ r3nzSkinStatusLabel;

 private:
  System::Windows::Forms::Label ^ gameStatusLabel;

 private:
  System::Windows::Forms::GroupBox ^ injectorStatusBox;

 private:
  System::Windows::Forms::GroupBox ^ clientStatusBox;

 private:
  System::Windows::Forms::Label ^ clientStatusLabel;

 private:
  System::Windows::Forms::GroupBox ^ gameStatusBox;

 private:
  System::Windows::Forms::GroupBox ^ r3nzSkinStatusBox;

 private:
  System::Windows::Forms::LinkLabel ^ linkLabel;

 private:
  System::ComponentModel::IContainer ^ components;

 private:
 private:
  System::Windows::Forms::NotifyIcon ^ trayIcon;

 private:
  System::Windows::Forms::ContextMenu ^ contextMenu;

 private:
  System::Windows::Forms::MenuItem ^ trayCloseButton;

 private:
  System::Windows::Forms::MenuItem ^ trayInjectButton;

 private:
  System::Windows::Forms::MenuItem ^ trayShowButton;

 private:
  System::Windows::Forms::MenuStrip ^ menuStrip;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ preferenceMenu;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ trayHideMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ autoInjectMenuItem;
#pragma region Windows Form Designer generated code
  void InitializeComponent(void) {
    this->components = (gcnew System::ComponentModel::Container());
    System::ComponentModel::ComponentResourceManager ^ resources =
      (gcnew System::ComponentModel::ComponentResourceManager(R3nzUI::typeid));
    this->injectButton        = (gcnew System::Windows::Forms::Button());
    this->injectorStatusLabel = (gcnew System::Windows::Forms::Label());
    this->r3nzSkinStatusLabel = (gcnew System::Windows::Forms::Label());
    this->gameStatusLabel     = (gcnew System::Windows::Forms::Label());
    this->injectorStatusBox   = (gcnew System::Windows::Forms::GroupBox());
    this->clientStatusBox     = (gcnew System::Windows::Forms::GroupBox());
    this->clientStatusLabel   = (gcnew System::Windows::Forms::Label());
    this->gameStatusBox       = (gcnew System::Windows::Forms::GroupBox());
    this->r3nzSkinStatusBox   = (gcnew System::Windows::Forms::GroupBox());
    this->linkLabel           = (gcnew System::Windows::Forms::LinkLabel());
    this->trayIcon            = (gcnew System::Windows::Forms::NotifyIcon(this->components));
    this->contextMenu         = (gcnew System::Windows::Forms::ContextMenu());
    this->trayCloseButton     = (gcnew System::Windows::Forms::MenuItem());
    this->trayInjectButton    = (gcnew System::Windows::Forms::MenuItem());
    this->trayShowButton      = (gcnew System::Windows::Forms::MenuItem());
    this->menuStrip           = (gcnew System::Windows::Forms::MenuStrip());
    this->preferenceMenu      = (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->trayHideMenuItem    = (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->autoInjectMenuItem  = (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->injectorStatusBox->SuspendLayout();
    this->clientStatusBox->SuspendLayout();
    this->gameStatusBox->SuspendLayout();
    this->r3nzSkinStatusBox->SuspendLayout();
    this->menuStrip->SuspendLayout();
    this->SuspendLayout();
    //
    // injectButton
    //
    this->injectButton->BackColor = System::Drawing::Color::FromArgb(
      static_cast<System::Int32>(static_cast<System::Byte>(245)),
      static_cast<System::Int32>(static_cast<System::Byte>(8)),
      static_cast<System::Int32>(static_cast<System::Byte>(83))
    );
    this->injectButton->Cursor                  = System::Windows::Forms::Cursors::Hand;
    this->injectButton->FlatStyle               = System::Windows::Forms::FlatStyle::Flat;
    this->injectButton->Font                    = (gcnew System::Drawing::Font(
      L"Arial", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->injectButton->Location                = System::Drawing::Point(12, 28);
    this->injectButton->Name                    = L"injectButton";
    this->injectButton->Size                    = System::Drawing::Size(250, 50);
    this->injectButton->TabIndex                = 0;
    this->injectButton->Text                    = L"Start";
    this->injectButton->UseVisualStyleBackColor = false;
    this->injectButton->Click += gcnew System::EventHandler(this, &R3nzUI::injectButton_Click);
    //
    // injectorStatusLabel
    //
    this->injectorStatusLabel->AutoSize  = true;
    this->injectorStatusLabel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->injectorStatusLabel->Font      = (gcnew System::Drawing::Font(
      L"Arial", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->injectorStatusLabel->ForeColor = System::Drawing::Color::FromArgb(
      static_cast<System::Int32>(static_cast<System::Byte>(245)),
      static_cast<System::Int32>(static_cast<System::Byte>(8)),
      static_cast<System::Int32>(static_cast<System::Byte>(83))
    );
    this->injectorStatusLabel->Location = System::Drawing::Point(6, 16);
    this->injectorStatusLabel->Name     = L"injectorStatusLabel";
    this->injectorStatusLabel->Size     = System::Drawing::Size(68, 18);
    this->injectorStatusLabel->TabIndex = 1;
    this->injectorStatusLabel->Text     = L"Stopped";
    //
    // r3nzSkinStatusLabel
    //
    this->r3nzSkinStatusLabel->AutoSize  = true;
    this->r3nzSkinStatusLabel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->r3nzSkinStatusLabel->Font      = (gcnew System::Drawing::Font(
      L"Arial", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->r3nzSkinStatusLabel->ForeColor = System::Drawing::Color::FromArgb(
      static_cast<System::Int32>(static_cast<System::Byte>(245)),
      static_cast<System::Int32>(static_cast<System::Byte>(8)),
      static_cast<System::Int32>(static_cast<System::Byte>(83))
    );
    this->r3nzSkinStatusLabel->Location = System::Drawing::Point(6, 16);
    this->r3nzSkinStatusLabel->Name     = L"r3nzSkinStatusLabel";
    this->r3nzSkinStatusLabel->Size     = System::Drawing::Size(94, 18);
    this->r3nzSkinStatusLabel->TabIndex = 2;
    this->r3nzSkinStatusLabel->Text     = L"Not Injected";
    //
    // gameStatusLabel
    //
    this->gameStatusLabel->AutoSize  = true;
    this->gameStatusLabel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->gameStatusLabel->Font      = (gcnew System::Drawing::Font(
      L"Arial", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->gameStatusLabel->ForeColor = System::Drawing::Color::FromArgb(
      static_cast<System::Int32>(static_cast<System::Byte>(245)),
      static_cast<System::Int32>(static_cast<System::Byte>(8)),
      static_cast<System::Int32>(static_cast<System::Byte>(83))
    );
    this->gameStatusLabel->Location = System::Drawing::Point(6, 16);
    this->gameStatusLabel->Name     = L"gameStatusLabel";
    this->gameStatusLabel->Size     = System::Drawing::Size(82, 18);
    this->gameStatusLabel->TabIndex = 3;
    this->gameStatusLabel->Text     = L"Not Found";
    //
    // injectorStatusBox
    //
    this->injectorStatusBox->Controls->Add(this->injectorStatusLabel);
    this->injectorStatusBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->injectorStatusBox->Font      = (gcnew System::Drawing::Font(
      L"Arial", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->injectorStatusBox->ForeColor = System::Drawing::Color::White;
    this->injectorStatusBox->Location  = System::Drawing::Point(12, 79);
    this->injectorStatusBox->Name      = L"injectorStatusBox";
    this->injectorStatusBox->Size      = System::Drawing::Size(250, 45);
    this->injectorStatusBox->TabIndex  = 5;
    this->injectorStatusBox->TabStop   = false;
    this->injectorStatusBox->Text      = L"Injector Status";
    //
    // clientStatusBox
    //
    this->clientStatusBox->Controls->Add(this->clientStatusLabel);
    this->clientStatusBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->clientStatusBox->Font      = (gcnew System::Drawing::Font(
      L"Arial", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->clientStatusBox->ForeColor = System::Drawing::Color::White;
    this->clientStatusBox->Location  = System::Drawing::Point(12, 130);
    this->clientStatusBox->Name      = L"clientStatusBox";
    this->clientStatusBox->Size      = System::Drawing::Size(250, 45);
    this->clientStatusBox->TabIndex  = 7;
    this->clientStatusBox->TabStop   = false;
    this->clientStatusBox->Text      = L"LeagueClient Status";
    //
    // clientStatusLabel
    //
    this->clientStatusLabel->AutoSize  = true;
    this->clientStatusLabel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->clientStatusLabel->Font      = (gcnew System::Drawing::Font(
      L"Arial", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->clientStatusLabel->ForeColor = System::Drawing::Color::FromArgb(
      static_cast<System::Int32>(static_cast<System::Byte>(245)),
      static_cast<System::Int32>(static_cast<System::Byte>(8)),
      static_cast<System::Int32>(static_cast<System::Byte>(83))
    );
    this->clientStatusLabel->Location = System::Drawing::Point(6, 16);
    this->clientStatusLabel->Name     = L"clientStatusLabel";
    this->clientStatusLabel->Size     = System::Drawing::Size(82, 18);
    this->clientStatusLabel->TabIndex = 0;
    this->clientStatusLabel->Text     = L"Not Found";
    //
    // gameStatusBox
    //
    this->gameStatusBox->Controls->Add(this->gameStatusLabel);
    this->gameStatusBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->gameStatusBox->Font      = (gcnew System::Drawing::Font(
      L"Arial", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->gameStatusBox->ForeColor = System::Drawing::Color::White;
    this->gameStatusBox->Location  = System::Drawing::Point(12, 181);
    this->gameStatusBox->Name      = L"gameStatusBox";
    this->gameStatusBox->Size      = System::Drawing::Size(250, 45);
    this->gameStatusBox->TabIndex  = 8;
    this->gameStatusBox->TabStop   = false;
    this->gameStatusBox->Text      = L"LeagueGame Status";
    //
    // r3nzSkinStatusBox
    //
    this->r3nzSkinStatusBox->Controls->Add(this->r3nzSkinStatusLabel);
    this->r3nzSkinStatusBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
    this->r3nzSkinStatusBox->Font      = (gcnew System::Drawing::Font(
      L"Arial", 6.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->r3nzSkinStatusBox->ForeColor = System::Drawing::Color::White;
    this->r3nzSkinStatusBox->Location  = System::Drawing::Point(12, 232);
    this->r3nzSkinStatusBox->Name      = L"r3nzSkinStatusBox";
    this->r3nzSkinStatusBox->Size      = System::Drawing::Size(250, 45);
    this->r3nzSkinStatusBox->TabIndex  = 9;
    this->r3nzSkinStatusBox->TabStop   = false;
    this->r3nzSkinStatusBox->Text      = L"R3nzSkin Status";
    //
    // linkLabel
    //
    this->linkLabel->AutoSize     = true;
    this->linkLabel->Cursor       = System::Windows::Forms::Cursors::Hand;
    this->linkLabel->LinkBehavior = System::Windows::Forms::LinkBehavior::NeverUnderline;
    this->linkLabel->LinkColor    = System::Drawing::Color::Silver;
    this->linkLabel->Location     = System::Drawing::Point(20, 284);
    this->linkLabel->Name         = L"linkLabel";
    this->linkLabel->Size         = System::Drawing::Size(240, 14);
    this->linkLabel->TabIndex     = 11;
    this->linkLabel->TabStop      = true;
    this->linkLabel->Text         = L"(c) 2021-2023 R3nzTheCodeGOD, NiceAesth";
    this->linkLabel->TextAlign    = System::Drawing::ContentAlignment::MiddleCenter;
    this->linkLabel->LinkClicked +=
      gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &R3nzUI::linkLabel_Click);
    //
    // trayIcon
    //
    this->trayIcon->ContextMenu = this->contextMenu;
    this->trayIcon->Icon        = (cli::safe_cast<System::Drawing::Icon ^>(resources->GetObject(L"trayIcon.Icon")));
    this->trayIcon->Text        = L"R3nzSkin";
    this->trayIcon->MouseDoubleClick +=
      gcnew System::Windows::Forms::MouseEventHandler(this, &R3nzUI::trayIcon_MouseDoubleClick);
    //
    // contextMenu
    //
    this->contextMenu->MenuItems->AddRange(gcnew cli::array<System::Windows::Forms::MenuItem ^>(3){
      this->trayCloseButton, this->trayInjectButton, this->trayShowButton});
    //
    // trayShowButton
    //
    this->trayShowButton->Index = 0;
    this->trayShowButton->Text  = L"Show";
    this->trayShowButton->Click += gcnew System::EventHandler(this, &R3nzUI::trayShowButton_Click);
    //
    // trayInjectButton
    //
    this->trayInjectButton->Index = 1;
    this->trayInjectButton->Text  = L"Start";
    this->trayInjectButton->Click += gcnew System::EventHandler(this, &R3nzUI::trayInjectButton_Click);
    //
    // trayCloseButton
    //
    this->trayCloseButton->Index = 2;
    this->trayCloseButton->Text  = L"Exit";
    this->trayCloseButton->Click += gcnew System::EventHandler(this, &R3nzUI::trayCloseButton_Click);
    //
    // menuStrip
    //
    this->menuStrip->Items->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(1){this->preferenceMenu}
    );
    this->menuStrip->Location = System::Drawing::Point(0, 0);
    this->menuStrip->Name     = L"menuStrip";
    this->menuStrip->Size     = System::Drawing::Size(273, 24);
    this->menuStrip->TabIndex = 0;
    //
    // preferenceMenu
    //
    this->preferenceMenu->DropDownItems->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(2){
      this->trayHideMenuItem, this->autoInjectMenuItem});
    this->preferenceMenu->Name = L"preferenceMenu";
    this->preferenceMenu->Size = System::Drawing::Size(80, 20);
    this->preferenceMenu->Text = L"Preferences";
    //
    // trayHideMenuItem
    //
    this->trayHideMenuItem->Name = L"trayHideMenuItem";
    this->trayHideMenuItem->Size = System::Drawing::Size(180, 22);
    this->trayHideMenuItem->Text = L"Hide to tray";
    this->trayHideMenuItem->Click += gcnew System::EventHandler(this, &R3nzUI::trayHideMenuItem_Click);
    //
    // autoInjectMenuItem
    //
    this->autoInjectMenuItem->Name = L"autoInjectMenuItem";
    this->autoInjectMenuItem->Size = System::Drawing::Size(180, 22);
    this->autoInjectMenuItem->Text = L"Auto Inject";
    this->autoInjectMenuItem->Click += gcnew System::EventHandler(this, &R3nzUI::autoInjectMenuItem_Click);
    //
    // R3nzUI
    //
    this->AutoScaleDimensions = System::Drawing::SizeF(7, 14);
    this->AutoScaleMode       = System::Windows::Forms::AutoScaleMode::Font;
    this->BackColor           = System::Drawing::Color::FromArgb(
      static_cast<System::Int32>(static_cast<System::Byte>(32)),
      static_cast<System::Int32>(static_cast<System::Byte>(30)),
      static_cast<System::Int32>(static_cast<System::Byte>(30))
    );
    this->ClientSize = System::Drawing::Size(273, 307);
    this->Controls->Add(this->menuStrip);
    this->Controls->Add(this->linkLabel);
    this->Controls->Add(this->injectorStatusBox);
    this->Controls->Add(this->gameStatusBox);
    this->Controls->Add(this->clientStatusBox);
    this->Controls->Add(this->r3nzSkinStatusBox);
    this->Controls->Add(this->injectButton);
    this->Cursor          = System::Windows::Forms::Cursors::Arrow;
    this->Font            = (gcnew System::Drawing::Font(
      L"Arial", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
      static_cast<System::Byte>(162)
    ));
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
    this->Icon            = (cli::safe_cast<System::Drawing::Icon ^>(resources->GetObject(L"$this.Icon")));
    this->MaximizeBox     = false;
    this->Name            = L"R3nzUI";
    this->RightToLeft     = System::Windows::Forms::RightToLeft::No;
    this->Text            = L"R3nzSkin";
    this->Load += gcnew System::EventHandler(this, &R3nzUI::R3nzUI_Load);
    this->Resize += gcnew System::EventHandler(this, &R3nzUI::R3nzUI_Resize);
    this->injectorStatusBox->ResumeLayout(false);
    this->injectorStatusBox->PerformLayout();
    this->clientStatusBox->ResumeLayout(false);
    this->clientStatusBox->PerformLayout();
    this->gameStatusBox->ResumeLayout(false);
    this->gameStatusBox->PerformLayout();
    this->r3nzSkinStatusBox->ResumeLayout(false);
    this->r3nzSkinStatusBox->PerformLayout();
    this->menuStrip->ResumeLayout(false);
    this->menuStrip->PerformLayout();
    this->ResumeLayout(false);
    this->PerformLayout();
  }
#pragma endregion
 private:
  System::Void R3nzUI_Load(System::Object ^ sender, System::EventArgs ^ e) {
    System::AppDomain::CurrentDomain->UnhandledException +=
      gcnew System::UnhandledExceptionEventHandler(this, &R3nzUI::R3nzUI_ExceptionHandler);
  }

 private:
  System::Void R3nzUI_ExceptionHandler(System::Object ^ sender, System::UnhandledExceptionEventArgs ^ e) {
    const auto exception = dynamic_cast<System::Exception ^>(e->ExceptionObject)->Message;
    MessageBox::Show(exception);
  }

 private:
  System::Void injectButton_Click(System::Object ^ sender, System::EventArgs ^ e) {
    btnState = !btnState;
    if (btnState) {
      this->injectButton->BackColor        = System::Drawing::Color::FromArgb(255, 252, 220, 107);
      this->injectorStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 252, 220, 107);
      this->injectButton->Text             = L"Stop";
      this->injectorStatusLabel->Text      = L"Working";
      this->trayInjectButton->Text         = L"Stop";
    } else {
      this->injectButton->BackColor        = System::Drawing::Color::FromArgb(255, 245, 8, 83);
      this->injectorStatusLabel->ForeColor = System::Drawing::Color::FromArgb(255, 245, 8, 83);
      this->injectButton->Text             = L"Start";
      this->injectorStatusLabel->Text      = L"Stopped";
      this->trayInjectButton->Text         = L"Start";
    }
  }

 private:
  System::Void linkLabel_Click(System::Object ^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs ^ e) {
    System::Diagnostics::Process::Start(L"https://github.com/NiceAesth/R3nzSkin");
  }

 private:
  System::Void R3nzUI_Resize(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->WindowState == FormWindowState::Minimized) {
      if (this->trayHideMenuItem->Checked) {
        this->Hide();
        this->trayIcon->Visible = true;
      }
    }
  }

 private:
  System::Void trayIcon_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    if (this->trayHideMenuItem->Checked) {
      this->Show();
      this->WindowState       = FormWindowState::Normal;
      this->trayIcon->Visible = false;
    }
  }

 private:
  System::Void trayShowButton_Click(System::Object ^ sender, System::EventArgs ^ e) {
    trayIcon_MouseDoubleClick(nullptr, nullptr);
  }

 private:
  System::Void trayInjectButton_Click(System::Object ^ sender, System::EventArgs ^ e) {
    this->injectButton_Click(nullptr, nullptr);
  }

 private:
  System::Void trayCloseButton_Click(System::Object ^ sender, System::EventArgs ^ e) {
    this->Close();
  }

 private:
  System::Void trayHideMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e) {
    this->trayHideMenuItem->Checked = !this->trayHideMenuItem->Checked;
    this->saveSettings();
  }

 private:
  System::Void autoInjectMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e) {
    this->autoInjectMenuItem->Checked = !this->autoInjectMenuItem->Checked;
    this->saveSettings();
  }
};
}  // namespace R3nzSkinInjector
