/*=========================================================================

  Program:   ParaView
  Module:    vtkPVServerFileDialog.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVServerFileDialog.h"

#include "vtkDirectory.h"
#include "vtkIntArray.h"
#include "vtkKWDirectoryUtilities.h"
#include "vtkKWCanvas.h"
#include "vtkKWEntry.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWListBox.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWWindow.h"
#include "vtkObjectFactory.h"
#include "vtkPVApplication.h"
#include "vtkPVProcessModule.h"
#include "vtkString.h"
#include "vtkStringList.h"
#include "vtkTclUtil.h"
#include "vtkClientServerStream.h"

#include <vtkstd/string>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkPVServerFileDialog );
vtkCxxRevisionMacro(vtkPVServerFileDialog, "1.33");

int vtkPVServerFileDialogCommand(ClientData cd, Tcl_Interp *interp,
                           int argc, char *argv[]);
int vtkStringListCommand(ClientData cd, Tcl_Interp *interp,
                         int argc, char *argv[]);


// Taken from source selection list  we need ne images.
/* 
 * This part was generated by ImageConvert from image:
 *    PVUpDirectoryButton.png (zlib, base64)
 */
#define image_PVUpDirectoryButton_width         16
#define image_PVUpDirectoryButton_height        16
#define image_PVUpDirectoryButton_pixel_size    3
#define image_PVUpDirectoryButton_buffer_length 108

static unsigned char image_PVUpDirectoryButton[] = 
  "eNq7cuHEFbIQAyogqPj/f5AaOIlHCwPRAK4e2WQ8JEQlmmPwaIerR1YMt5qg+ZiuJWg+RJ"
  "B27ocI4nIYVvPxkFjNJyb8SY1i2iEAqg6qMg==";


/* 
 * This part was generated by ImageConvert from image:
 *    PVFolder.png (zlib, base64)
 */
#define image_PVFolder_width         17
#define image_PVFolder_height        17
#define image_PVFolder_pixel_size    3
#define image_PVFolder_buffer_length 448

static unsigned char image_PVFolder[] = 
  "eNr7/59+4MxMk9MzjE9OMzw+Rf/+/fvfvn0jqB6Ze6hf68SJE3h0wdTPREL/93ap7mpX2t"
  "4iv7VJdnOj9IY6CaDV379/h2gBuuf/94n/v/ShIFSwrkasp6fn3r17EC7Q/f9ft/5/1nh0"
  "ki4aOtCrsadTZUerwpZGGaBdQAS0C6gF6N//D6qBCvD59lkjkFhcyA20C8gAKb6SD/bRTF"
  "zox9EEILkgjyMvLw8SPv/PpIN8BJQFegober8nHCg7L4ctNzcXqAXoYKAWsMNmgjyFDT3b"
  "7AeUnZPFAtECDE+glv096iAtD6qxojsrXYCyszKYIFqA4Q/UAgwWkJYr+VjR5QXWQNmZ6Y"
  "wQvwDDDR5fwPBfXSm8okxgaTEvMHyA/gW6H+geoPlA9T4+Ps3NzUAtwIQBTB5Adi4hAFQD"
  "VAkJdaAuoF3XCAFikisuAADhFaXg";

/* 
 * This part was generated by ImageConvert from image:
 *    PVDocument.png (zlib, base64)
 */
#define image_PVDocument_width         17
#define image_PVDocument_height        17
#define image_PVDocument_pixel_size    3
#define image_PVDocument_buffer_length 176

static unsigned char image_PVDocument[] = 
  "eNqd0sENgCAMBdAeu4+zdB9O7sBMcHYFxqgFYiS1FOJPA8b0pWBk/pPTTYzRJKVcsxJCRF"
  "/inKFPUWpJ6IkiznUQMaVkEoDI+ZBVlRB1tpGY5RDzLvLRVgQCQH/f9yWp/a217m3dmMLv"
  "FHkK4JPZDzAj6MYkyyiSNzIS2k7vvwHIB2gH";


// This constant must be adjusted to be the largest width of the icons
// to be used on the left size of a label.

#define image_icon_max_width         image_PVFolder_width

//----------------------------------------------------------------------------
vtkPVServerFileDialog::vtkPVServerFileDialog()
{
  this->CommandFunction = vtkPVServerFileDialogCommand;
  
  this->TopFrame = vtkKWWidget::New();
  this->MiddleFrame = vtkKWFrame::New();
  this->FileList = vtkKWCanvas::New();
  this->BottomFrame = vtkKWWidget::New();

  this->DirectoryDisplay = vtkKWLabel::New();
  this->DirectoryMenuButton = vtkKWMenuButton::New();  

  this->DownDirectoryButton = vtkKWLabel::New();

  this->FileNameLabel = vtkKWLabel::New();
  this->FileNameEntry = vtkKWEntry::New();
  this->FileNameMenuButton = vtkKWMenuButton::New();

  this->ExtensionsLabel = vtkKWLabel::New();
  this->ExtensionsDisplayFrame = vtkKWWidget::New();
  this->ExtensionsDisplay = vtkKWLabel::New();
  this->ExtensionsMenuButton = vtkKWMenuButton::New();

  this->LoadSaveButton = vtkKWPushButton::New();
  this->CancelButton = vtkKWPushButton::New();

  this->MasterWindow = 0;

  this->SelectBoxId = NULL;
  this->SelectedDirectory = NULL;

  this->FileTypeStrings = vtkStringList::New();
  this->FileTypeDescriptions = vtkStringList::New();
  this->ExtensionStrings = vtkStringList::New();

  this->ScrollBar = vtkKWWidget::New();
  this->ServerSideID.ID = 0;
}

//----------------------------------------------------------------------------
vtkPVServerFileDialog::~vtkPVServerFileDialog()
{
  this->TopFrame->Delete();
  this->TopFrame = NULL;
  this->MiddleFrame->Delete();
  this->MiddleFrame = NULL;
  this->FileList->Delete();
  this->FileList = NULL;
  this->BottomFrame->Delete();
  this->BottomFrame = NULL;

  this->DirectoryDisplay->Delete();
  this->DirectoryDisplay = NULL;
  this->DirectoryMenuButton->Delete();
  this->DirectoryMenuButton = NULL; 

  this->DownDirectoryButton->Delete();
  this->DownDirectoryButton = NULL;

  this->FileNameLabel->Delete();
  this->FileNameLabel = NULL;
  this->FileNameEntry->Delete();
  this->FileNameEntry = NULL;
  this->FileNameMenuButton->Delete();
  this->FileNameMenuButton = NULL;

  this->ExtensionsLabel->Delete();
  this->ExtensionsLabel = NULL;
  this->ExtensionsDisplayFrame->Delete();
  this->ExtensionsDisplayFrame = NULL;
  this->ExtensionsDisplay->Delete();
  this->ExtensionsDisplay = NULL;
  this->ExtensionsMenuButton->Delete();
  this->ExtensionsMenuButton = NULL;

  this->LoadSaveButton->Delete();
  this->LoadSaveButton = NULL;
  this->CancelButton->Delete();
  this->CancelButton = NULL;

  this->SetMasterWindow(0);
  
  this->SetSelectBoxId(NULL);

  this->FileTypeStrings->Delete();
  this->FileTypeStrings = NULL;

  this->FileTypeDescriptions->Delete();
  this->FileTypeDescriptions = NULL;

  this->ExtensionStrings->Delete();
  this->ExtensionStrings = NULL;

  if (this->ScrollBar)
    {
    this->ScrollBar->Delete();
    }

  if(this->ServerSideID.ID)
    {
    vtkPVProcessModule* pm = this->GetPVApplication()->GetProcessModule();
    pm->DeleteStreamObject(this->ServerSideID);
    pm->SendStream(vtkProcessModule::DATA_SERVER_ROOT);
    }
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::CreateServerSide()
{
  if(!this->ServerSideID.ID)
    {
    vtkPVProcessModule* pm = this->GetPVApplication()->GetProcessModule();
    this->ServerSideID = pm->NewStreamObject("vtkPVServerFileListing");
    pm->SendStream(vtkProcessModule::DATA_SERVER_ROOT);
    }
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::SetMasterWindow(vtkKWWindow* win)
{
  if (this->MasterWindow != win) 
    { 
    this->MasterWindow = win; 
    if (this->MasterWindow) 
      { 
      if (this->IsCreated())
        {
        this->Script("wm transient %s %s", this->GetWidgetName(), 
                     this->MasterWindow->GetWidgetName());
        }
      } 
    this->Modified(); 
    }   
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::Create(vtkKWApplication *app, const char *)
{
  // Call the superclass to set the appropriate flags then create manually

  if (!this->vtkKWWidget::Create(app, NULL, NULL))
    {
    vtkErrorMacro("Failed creating widget " << this->GetClassName());
    return;
    }

  const char *wname = this->GetWidgetName();
  if (this->MasterWindow)
    {
    this->Script("toplevel %s -class %s", 
                 wname, 
                 this->MasterWindow->GetClassName());
    }
  else
    {
    this->Script("toplevel %s", wname);
    }

  if (this->SaveDialog)
    {
    this->SetTitle("ParaView Save File");
    }
  else
    {
    this->SetTitle("ParaView Load File");
    }
  this->Script("wm title %s \"%s\"", wname, this->Title);
  this->Script("wm iconname %s \"vtk\"", wname);
  if (this->MasterWindow)
    {
    this->Script("wm transient %s %s", wname, 
                 this->MasterWindow->GetWidgetName());
    }


  this->Script("wm protocol %s WM_DELETE_WINDOW {%s CancelCallback}",
               wname, this->GetTclName());
    
  this->TopFrame->SetParent(this);
  this->TopFrame->Create(app, "frame", "");
  this->Script("grid %s -column 0 -row 0 -padx 2m -sticky ew",
               this->TopFrame->GetWidgetName());

  this->MiddleFrame->SetParent(this);
  this->MiddleFrame->Create(app, " ");
  this->Script("grid %s -column 0 -row 1 -padx 2m -sticky ewns",
               this->MiddleFrame->GetWidgetName());

  this->FileList->SetParent(this->MiddleFrame); 
  this->FileList->Create(app, "-background white -bd 2 -relief sunken"); 
  this->Script("pack %s -fill both -expand 1",
               this->FileList->GetWidgetName());

  this->Script("grid %s -row 0 -column 0 -sticky news", 
               this->FileList->GetWidgetName());
  this->Script("grid columnconfig %s 0 -weight 1", 
               this->MiddleFrame->GetWidgetName());
  this->Script("grid rowconfig %s 0 -weight 1", 
               this->MiddleFrame->GetWidgetName());

  // Scrollbar
  ostrstream command;
  this->ScrollBar->SetParent(this->MiddleFrame);
  command << "-command \"" <<  this->FileList->GetWidgetName()
          << " yview\"" << ends;
  char* commandStr = command.str();
  this->ScrollBar->Create(this->GetApplication(), "scrollbar", commandStr);
  delete[] commandStr;
  this->Script("%s configure -yscrollcommand \"%s set\"", 
               this->FileList->GetWidgetName(),
               this->ScrollBar->GetWidgetName());
  this->FileList->SetBind(this, "<Configure>", "Reconfigure");


  this->BottomFrame->SetParent(this);
  this->BottomFrame->Create(app, "frame", "");
  this->Script("grid %s -column 0 -row 2 -pady 1m  -padx 2m -sticky ew",
               this->BottomFrame->GetWidgetName());

  // Make the file list the only row which resizes.
  this->Script("grid rowconfigure %s 1 -weight 1 -minsize 250",
               this->GetWidgetName());
  // Resize all horizontally.
  this->Script("grid columnconfigure %s 0 -weight 1 -minsize 500",
               this->GetWidgetName());

  this->DirectoryDisplay->SetParent(this->TopFrame);
  this->DirectoryDisplay->Create(app, "-background white -bd 2 -relief sunken");
  this->Script("grid %s -row 0 -column 0 -pady 2m -sticky news",
               this->DirectoryDisplay->GetWidgetName());

  this->DownDirectoryButton->SetParent(this->TopFrame);
  this->DownDirectoryButton->Create(app, "");
  this->DownDirectoryButton->SetImageOption(
    image_PVUpDirectoryButton, 
    image_PVUpDirectoryButton_width,
    image_PVUpDirectoryButton_height,
    image_PVUpDirectoryButton_pixel_size,
    image_PVUpDirectoryButton_buffer_length);

  this->DownDirectoryButton->SetBalloonHelpString("Up One Level");
  this->Script("bind %s <ButtonRelease-1> { %s DownDirectoryCallback}",
               this->DownDirectoryButton->GetWidgetName(),
               this->GetTclName());

  this->Script("grid %s -row 0 -column 1 -pady 2m -padx 5m -sticky news",
               this->DownDirectoryButton->GetWidgetName());
  this->Script("grid columnconfigure %s 0 -weight 1",
               this->TopFrame->GetWidgetName());


  this->FileNameLabel->SetParent(this->BottomFrame);
  this->FileNameLabel->SetLabel("File name:");
  this->FileNameLabel->Create(app, "");
  this->Script("grid %s -row 0 -column 0 -sticky w", 
               this->FileNameLabel->GetWidgetName());

  this->FileNameEntry->SetParent(this->BottomFrame);
  this->FileNameEntry->Create(app, "");
  this->Script("grid %s -row 0 -column 1 -sticky ew -padx 5m", 
               this->FileNameEntry->GetWidgetName());

  this->LoadSaveButton->SetParent(this->BottomFrame);
  this->LoadSaveButton->Create(app, "");
  this->LoadSaveButton->SetCommand(this, "LoadSaveCallback");

  if (this->SaveDialog)
    {
    this->LoadSaveButton->SetLabel("Save");
    }
  else
    {
    this->LoadSaveButton->SetLabel("Open");
    }
  this->Script("grid %s -row 0 -column 2 -sticky ew -ipadx 2m", 
               this->LoadSaveButton->GetWidgetName());


  this->ExtensionsLabel->SetParent(this->BottomFrame);
  if (this->SaveDialog)
    {
    this->ExtensionsLabel->SetLabel("Save as type:");
    }
  else
    {
    this->ExtensionsLabel->SetLabel("Load type:");
    }
  this->ExtensionsLabel->Create(app, "");
  this->Script("grid %s -row 1 -column 0 -sticky w", 
               this->ExtensionsLabel->GetWidgetName());

  this->ExtensionsDisplayFrame->SetParent(this->BottomFrame);
  this->ExtensionsDisplayFrame->Create(app, "frame", "-bd 2 -relief sunken");
  this->Script("grid %s -row 1 -column 1  -sticky ew -padx 5m", 
               this->ExtensionsDisplayFrame->GetWidgetName());
  this->ExtensionsDisplay->SetParent(this->ExtensionsDisplayFrame);
  this->ExtensionsDisplay->Create(app, "-background white");
  this->Script("pack %s -side left -expand 1 -fill both",
               this->ExtensionsDisplay->GetWidgetName());
  this->ExtensionsMenuButton->SetParent(this->ExtensionsDisplayFrame);
  this->ExtensionsMenuButton->GetMenu()->SetTearOff(0);
  this->ExtensionsMenuButton->Create(app, "");
  this->ExtensionsMenuButton->SetButtonText("");
  this->Script("pack %s -side left -expand 0", 
               this->ExtensionsMenuButton->GetWidgetName());

  this->CancelButton->SetParent(this->BottomFrame);
  this->CancelButton->Create(app, "-pady 0");
  this->CancelButton->SetCommand(this, "CancelCallback");
  this->CancelButton->SetLabel("Cancel");
  this->Script("grid %s -row 1 -column 2 -sticky ew -ipadx 2m", 
               this->CancelButton->GetWidgetName());

  this->Script("grid columnconfigure %s 1 -weight 1",
               this->BottomFrame->GetWidgetName());

  this->Script("wm withdraw %s", wname);


  // Icons
  ostrstream folder;
  folder << this->GetWidgetName() << ".folderimg" << ends;
  if (!vtkKWTkUtilities::UpdatePhoto(this->GetApplication()->GetMainInterp(),
                                     folder.str(), 
                                     image_PVFolder, 
                                     image_PVFolder_width, 
                                     image_PVFolder_height, 
                                     image_PVFolder_pixel_size,
                                     image_PVFolder_buffer_length,
                                     this->FileList->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (eye open)");
    }
  folder.rdbuf()->freeze(0);

  ostrstream document;
  document << this->GetWidgetName() << ".documentimg" << ends;
  if (!vtkKWTkUtilities::UpdatePhoto(this->GetApplication()->GetMainInterp(),
                                     document.str(), 
                                     image_PVDocument, 
                                     image_PVDocument_width, 
                                     image_PVDocument_height, 
                                     image_PVDocument_pixel_size,
                                     image_PVDocument_buffer_length,
                                     this->FileList->GetWidgetName()))
    {
    vtkWarningMacro(<< "Error creating photo (eye gray)");
    }
  document.rdbuf()->freeze(0);

}

//----------------------------------------------------------------------------
int vtkPVServerFileDialog::Invoke()
{   
  // Get rid of back slashes.
  this->ConvertLastPath();

  this->GetApplication()->SetDialogUp(1);
  this->UpdateExtensionsMenu();
  // Side effect of UpdateExtensionsMenu is to Update.
  //this->Update();

  int width, height;

  int x, y;
  int sw, sh;
  sscanf(this->Script("concat [winfo screenwidth .] [winfo screenheight .]"),
         "%d %d", &sw, &sh);
  
  if (this->MasterWindow)
    {
    this->Script("wm geometry %s", this->MasterWindow->GetWidgetName());
    sscanf(this->GetApplication()->GetMainInterp()->result, "%dx%d+%d+%d",
           &width, &height, &x, &y);
    
    x += width / 2;
    y += height / 2;
    
    if (x > sw - 200)
      {
      x = sw / 2;
      }
    if (y > sh - 200)
      {
      y = sh / 2;
      }
    }
  else
    {
    x = sw / 2;
    y = sh / 2;
    }

  sscanf(this->Script("concat [winfo reqwidth %s] [winfo reqheight %s]", 
                      this->GetWidgetName(), this->GetWidgetName()), 
         "%d %d", &width, &height);

  if (x > width / 2)
    {
    x -= width / 2;
    }
  if (y > height / 2)
    {
    y -= height / 2;
    }

  this->Script("wm geometry %s +%d+%d", this->GetWidgetName(),
               x, y);

  this->Script("wm deiconify %s", this->GetWidgetName());
  this->Script("grab %s", this->GetWidgetName());

  this->Done = 0;
  while ( ! this->Done)
    {
    this->Script("after 100; update");
    }
  this->GetApplication()->SetDialogUp(0);
  return this->ReturnValue;
}


//----------------------------------------------------------------------------
void vtkPVServerFileDialog::ConvertLastPath()
{  
  int max = 1000; 
  char *p;

  if (this->LastPath == NULL)
    {
    return;
    }
  p = this->LastPath;
  while (*p != '\0' && max > 0)
    {
    if (*p == '\\')
      {
      *p = '/';
      }
    --max;
    ++p;
    }
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::LoadSaveCallback()
{
  int last;
  const char* dir;
  dir = this->DirectoryDisplay->GetLabel();
  last = static_cast<int>(strlen(dir))-1;

  if (this->SelectedDirectory)
    {
    ostrstream newdir;
    if (strcmp(dir, "Available Drives") == 0)
      {
      newdir << this->SelectedDirectory << ends;
      }
    else if (last >= 0 && dir[last] == '/')
      { // special case for root. Avoid "//" in path.
      newdir << dir << this->SelectedDirectory << ends;
      }
    else
      {
      newdir << dir << "/" << this->SelectedDirectory << ends;
      }
    this->SetLastPath(newdir.str());
    this->ConvertLastPath();
    this->Update();
    newdir.rdbuf()->freeze(0);
    this->SetSelectedDirectory(0);
    return;
    }
  
  vtkstd::string fileName = this->FileNameEntry->GetValue();
  
  vtkPVProcessModule* pm = this->GetPVApplication()->GetProcessModule();
  if(fileName[0] == '/' || fileName[1] == ':')
    {
    this->CreateServerSide();
    pm->GetStream() << vtkClientServerStream::Invoke
                    << this->ServerSideID << "FileIsDirectory"
                    << fileName.c_str()
                    << vtkClientServerStream::End;
    pm->SendStream(vtkProcessModule::DATA_SERVER_ROOT);
    int isdir = 0;
    if(!pm->GetLastServerResult().GetArgument(0, 0, &isdir))
      {
      vtkErrorMacro("Error checking whether file is directory on server.");
      }
    if(isdir)
      {
      this->FileNameEntry->SetValue("");
      this->SetLastPath(fileName.c_str());
      this->ConvertLastPath();
      this->Update();
      return;
      }
    }
  
  vtkstd::string::size_type pos = fileName.find_last_of("/.");
  if(!(pos != fileName.npos && fileName[pos] == '.'))
    {
    // Need to add an extension.  Choose the first non-wildcard
    // extension.
    int i;
    const char* ext = 0;
    for(i = 0; i < this->ExtensionStrings->GetNumberOfStrings() && !ext; ++i)
      {
      const char* e = this->ExtensionStrings->GetString(i);
      if(strcmp(e, "*") != 0)
        {
        ext = e;
        }
      }
    
    if(ext)
      {
      fileName += ".";
      fileName += ext;
      }
    }
    
  ostrstream fullpath;
  if (last >= 0 && dir[last] == '/')
    { // special case for root. Avoid "//" in path.
    fullpath << dir << fileName.c_str() << ends;
    }
  else
    {
    fullpath << dir << "/" << fileName.c_str() << ends;
    }
  this->SetFileName(fullpath.str());
  fullpath.rdbuf()->freeze(0);
   
  this->Done = 1;
  this->Script("grab release %s", this->GetWidgetName());
  this->Script("wm withdraw %s", this->GetWidgetName());
  this->ReturnValue = 1;
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::CancelCallback()
{
  this->Done = 1;
  this->Script("grab release %s", this->GetWidgetName());
  this->Script("wm withdraw %s", this->GetWidgetName());
  this->ReturnValue = 0;
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::DownDirectoryCallback()
{
  // If this is the top-level directory on a Windows drive letter or
  // network drive, present a list of drive letters.
  int numSlashes=0;
  for(const char* lpp = this->LastPath; *lpp; ++lpp)
    {
    if(*lpp == '/')
      {
      ++numSlashes;
      }
    }
  if(((this->LastPath[0] != '/') &&
      (this->LastPath[1] == ':') &&
      (this->LastPath[2] == '/') &&
      (this->LastPath[3] == 0)) ||
     ((this->LastPath[0] == '/') &&
      (this->LastPath[1] == '/') &&
      (numSlashes == 3)))
    {
    this->SetLastPath("<GET_DRIVE_LETTERS>");
    this->Update();
    return;
    }
  
  char* newdir = new char[strlen(this->LastPath) + 1];
  strcpy(newdir, this->LastPath);
  
  int count = 0;
  char* last = newdir;
  for(char* p = newdir; *p ; ++p)
    {
    if(*p == '/')
      {
      last = p;
      ++count;
      }
    }
  
  if(count > 1)
    {
    *last = '\0';
    }
  else if(count == 1)
    {
    *(last+1) = '\0';
    }
  this->SetLastPath(newdir);
  
  delete [] newdir;
  this->Update();
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::ExtensionsMenuButtonCallback(int typeIdx)
{
  // Clear previous extensions.
  this->ExtensionStrings->RemoveAllItems();

  vtkstd::string extensions = this->FileTypeStrings->GetString(typeIdx);
  for(unsigned int i = 0; i < extensions.length(); ++i)
    {
    if(i < extensions.length() && extensions[i] == '*')
      {
      ++i;
      }
    if(i < extensions.length() && extensions[i] == '.')
      {
      ++i;
      }
    
    unsigned int extensionStart = i;
#ifdef _WIN32
    while(i < extensions.length() && extensions[i] != ';')
#else
    while(i < extensions.length() && extensions[i] != ' ')
#endif
      {
      ++i;
      }
    
    if(i > extensionStart)
      {
      vtkstd::string ext = extensions.substr(extensionStart,
                                             i - extensionStart);
      this->ExtensionStrings->AddString(ext.c_str());
      }
    }
  
  ostrstream label;
  label << this->FileTypeDescriptions->GetString(typeIdx) << " ";
  if (this->ExtensionStrings->GetNumberOfStrings() <= 1)
    {
    label << extensions.c_str();
    }
  label << ends;

  this->ExtensionsDisplay->SetLabel(label.str());
  label.rdbuf()->freeze(0);
  
  this->Update();
}


//----------------------------------------------------------------------------
void vtkPVServerFileDialog::UpdateExtensionsMenu()
{
  char methodAndArgString[256];

  if (this->FileTypes == NULL)
    {
    return;
    }
  this->FileTypeStrings->RemoveAllItems();
  this->ExtensionsMenuButton->GetMenu()->DeleteAllMenuItems();
  
  this->FileTypeDescriptions->RemoveAllItems();
  this->FileTypeStrings->RemoveAllItems();

  this->Script(
    "namespace eval ::paraview::vtkPVSeverFileDialog {\n"
    "  proc ParseFileTypes {types dialog} {\n"
    "    foreach t $types {\n"
    "      $dialog AddDescriptionString [lindex $t 0]\n"
    "      $dialog AddExtensionString [lindex $t 1]\n"
    "    }\n"
    "  }\n"
    "  ParseFileTypes {%s} %s\n"
    "}\n", this->FileTypes, this->GetTclName());
  
  const unsigned int maxExtensionsLength = 16;
  for(int i=0; i < this->FileTypeStrings->GetNumberOfStrings(); ++i)
    {
    ostrstream label;
    vtkstd::string exts = this->FileTypeStrings->GetString(i);
    if(exts.length() > maxExtensionsLength)
      {
      vtkstd::string::size_type pos = exts.rfind(";", maxExtensionsLength);
      if(pos != vtkstd::string::npos)
        {
        exts = exts.substr(0, pos);
        exts += ";...";
        }
      }
    
    if(exts[0] == '.')
      {
      exts = "*"+exts;
      }
    
    label << this->FileTypeDescriptions->GetString(i) << " ("
          << exts.c_str() << ")" << ends;
    sprintf(methodAndArgString, "ExtensionsMenuButtonCallback %d", i);
    this->ExtensionsMenuButton->GetMenu()->AddCommand(
      label.str(), this, methodAndArgString);
    label.rdbuf()->freeze(0);
    }  
  
  // Default file type is the first in the list.
  if (this->FileTypeStrings->GetNumberOfStrings() > 0)
    {
    this->ExtensionsMenuButtonCallback(0);
    }
}

//----------------------------------------------------------------------------
int vtkPVServerFileDialog::CheckExtension(const char* name)
{
  int num;
  int idx;
  const char *nameExt;
  const char* ptr;
  const char *ext;

  if (name == NULL)
    {
    return 0;
    }

  num = this->ExtensionStrings->GetNumberOfStrings();
  if (num == 0)
    { // No extensions?  Pass all.
    return 1;
    }

  // Find the extension in the name.
  nameExt = NULL;
  ptr = name;
  while (*ptr != '\0')
    {
    if (*ptr == '.')
      {
      nameExt = ptr + 1;
      }
    ++ptr;
    }
  if (nameExt == NULL || *nameExt == '\0')
    {
    return 0;
    }
  for (idx = 0; idx < num; ++idx)
    {
    ext = this->ExtensionStrings->GetString(idx);
    if (strcmp(ext, "*") == 0)
      { // special case for All Files wild card.
      return 1;
      }
    if (strcmp(nameExt, ext) == 0)
      {
      return 1;
      }
    }

  return 0;
}

//----------------------------------------------------------------------------
vtkPVApplication* vtkPVServerFileDialog::GetPVApplication()
{
  return vtkPVApplication::SafeDownCast(this->GetApplication());
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::Update()
{
  vtkPVProcessModule* pm = this->GetPVApplication()->GetProcessModule();
  vtkStringList* dirs = vtkStringList::New();
  vtkStringList* files = vtkStringList::New();

  // Make sure we have a directory.
  if(!this->LastPath)
    {
    this->CreateServerSide();
    pm->GetStream() << vtkClientServerStream::Invoke
                    << this->ServerSideID << "GetCurrentWorkingDirectory"
                    << vtkClientServerStream::End;
    pm->SendStream(vtkProcessModule::DATA_SERVER_ROOT);
    const char* cwd;
    if(!pm->GetLastServerResult().GetArgument(0, 0, &cwd))
      {
      vtkErrorMacro("Error getting current working directory from server.");
      cwd = "";
      }
    this->SetLastPath(cwd);
    this->ConvertLastPath();
    }

  // Read the list of subdirectories and files.
  if(!(pm->GetDirectoryListing(this->LastPath, dirs, files, this->SaveDialog)))
    {
    // Directory did not exist, use current directory instead.
    this->CreateServerSide();
    pm->GetStream() << vtkClientServerStream::Invoke
                    << this->ServerSideID << "GetCurrentWorkingDirectory"
                    << vtkClientServerStream::End;
    pm->SendStream(vtkProcessModule::DATA_SERVER_ROOT);
    const char* cwd;
    if(!pm->GetLastServerResult().GetArgument(0, 0, &cwd))
      {
      vtkErrorMacro("Error getting current working directory from server.");
      cwd = "";
      }
    this->SetLastPath(cwd);
    this->ConvertLastPath();

    // We will now succeed.
    pm->GetDirectoryListing(this->LastPath, dirs, files, this->SaveDialog);
    }
  
  this->Script("%s delete all", this->FileList->GetWidgetName());
  this->SetSelectBoxId(NULL);
  if(this->LastPath[0] != '<')
    {
    this->DirectoryDisplay->SetLabel(this->LastPath);
    }
  else
    {
    this->DirectoryDisplay->SetLabel("Available Drives");
    }
  
  // Construct the gui representation.
  int y = 10;
  int dirsLen = dirs->GetLength();
  int i;
  for(i=0; i < dirsLen; ++i)
    {
    y = this->Insert(dirs->GetString(i), y, 1);
    }
  int filesLen = files->GetLength();
  for(i=0; i < filesLen; ++i)
    {
    const char* name = files->GetString(i);
    if(this->CheckExtension(name))
      {
      y = this->Insert(name, y, 0);
      }
    }
  
  files->Delete();
  dirs->Delete();

  this->Reconfigure();
}


//----------------------------------------------------------------------------
int vtkPVServerFileDialog::Insert(const char* name, int y, int directory)
{
  int x=10;
  int yNext = y+17;
  static const char *font = "-adobe-helvetica-medium-r-normal-*-14-100-100-100-p-76-iso8859-1";
  char *result = 0;
  char *tmp;

  // Create an image on the left of the label.
  this->Script("%s create image %d %d", this->FileList->GetWidgetName(), 
               x + image_icon_max_width / 2, y);
  if (this->GetApplication()->GetMainInterp()->result)
    {
    tmp = 
      vtkString::Duplicate(this->GetApplication()->GetMainInterp()->result);
    if (directory)
      {
      this->Script("%s bind %s <ButtonPress-1> {%s SelectDirectory {%s} %s}",
                   this->FileList->GetWidgetName(), tmp,
                   this->GetTclName(), name, tmp);
      this->Script("%s bind %s <Double-ButtonPress-1> {%s SelectDirectory {%s} %s; %s LoadSaveCallback}",
                   this->FileList->GetWidgetName(), tmp,
                   this->GetTclName(), name, tmp, this->GetTclName());
      this->Script("%s itemconfigure %s -image %s.folderimg",
                   this->FileList->GetWidgetName(), tmp, 
                   this->GetWidgetName());
      }
    else
      {
      this->Script("%s bind %s <ButtonPress-1> {%s SelectFile {%s} %s}",
                   this->FileList->GetWidgetName(), tmp,
                   this->GetTclName(), name, tmp);
      this->Script("%s bind %s <Double-ButtonPress-1> {%s SelectFile {%s} %s; %s LoadSaveCallback}",
                   this->FileList->GetWidgetName(), tmp,
                   this->GetTclName(), name, tmp, this->GetTclName());
      this->Script("%s itemconfigure %s -image %s.documentimg",
                   this->FileList->GetWidgetName(), tmp, 
                   this->GetWidgetName());
      }
    delete [] tmp;
    }

  // Add some space between the icon and the label

  x += image_icon_max_width + 4;

  // Draw the name of the file.
  this->Script("%s create text %d %d -text {%s} -font %s -anchor w -tags x",
               this->FileList->GetWidgetName(), x, y, name, font);

  // Make the name hot for picking.
  result = this->GetApplication()->GetMainInterp()->result;
  tmp = new char[strlen(result)+1];
  strcpy(tmp,result);
  if (directory)
    {
    this->Script("%s bind %s <ButtonPress-1> {%s SelectDirectory {%s} %s}",
                 this->FileList->GetWidgetName(), tmp,
                 this->GetTclName(), name, tmp);
    this->Script("%s bind %s <Double-ButtonPress-1> {%s SelectDirectory {%s} %s; %s LoadSaveCallback}",
                 this->FileList->GetWidgetName(), tmp,
                 this->GetTclName(), name, tmp, this->GetTclName());
    }
  else
    {
    this->Script("%s bind %s <ButtonPress-1> {%s SelectFile {%s} %s}",
                 this->FileList->GetWidgetName(), tmp,
                 this->GetTclName(), name, tmp);
    this->Script("%s bind %s <Double-ButtonPress-1> {%s SelectFile {%s} %s; %s LoadSaveCallback}",
                 this->FileList->GetWidgetName(), tmp,
                 this->GetTclName(), name, tmp, this->GetTclName());
    }
  delete [] tmp;
  return yNext;
}

  
//----------------------------------------------------------------------------
void vtkPVServerFileDialog::SelectFile(const char* name, const char* id)
{
  int bbox[4];
  const char* result;

  if (this->SelectBoxId)
    {
    this->Script("%s delete %s", this->FileList->GetWidgetName(), this->SelectBoxId);
    this->SetSelectBoxId(NULL);
    }

  // Get the bounding box for the name. We need to highlight it.
  this->Script( "%s bbox %s",this->FileList->GetWidgetName(), id);
  result = this->GetApplication()->GetMainInterp()->result;
  sscanf(result, "%d %d %d %d", bbox, bbox+1, bbox+2, bbox+3);

  this->Script("%s create rectangle %d %d %d %d -fill yellow -outline {}",
               this->FileList->GetWidgetName(), 
               bbox[0], bbox[1], bbox[2], bbox[3]);
  this->SetSelectBoxId(this->GetApplication()->GetMainInterp()->result);
  this->Script( "%s lower %s",this->FileList->GetWidgetName(), this->SelectBoxId);

  this->FileNameEntry->SetValue(name);

  this->SetSelectedDirectory(NULL);
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::SelectDirectory(const char* name, const char* id)
{
  int bbox[4];
  const char* result;

  if (this->SelectBoxId)
    {
    this->Script("%s delete %s", this->FileList->GetWidgetName(), this->SelectBoxId);
    this->SetSelectBoxId(NULL);
    }

  // Get the bounding box for the name. We need to highlight it.
  this->Script( "%s bbox %s",this->FileList->GetWidgetName(), id);
  result = this->GetApplication()->GetMainInterp()->result;
  sscanf(result, "%d %d %d %d", bbox, bbox+1, bbox+2, bbox+3);

  this->Script("%s create rectangle %d %d %d %d -fill yellow -outline {}",
               this->FileList->GetWidgetName(), 
               bbox[0], bbox[1], bbox[2], bbox[3]);
  this->SetSelectBoxId(this->GetApplication()->GetMainInterp()->result);
  this->Script( "%s lower %s",this->FileList->GetWidgetName(), this->SelectBoxId);

  this->FileNameEntry->SetValue("");
  this->SetSelectedDirectory(name);
}


//-----------------------------------------------------------------------------
void vtkPVServerFileDialog::Reconfigure()
{
  int bbox[4];
  this->CalculateBBox(this->FileList, "all", bbox);
  int height = atoi(this->Script("winfo height %s", 
                                 this->FileList->GetWidgetName()));
  if (height > 1 && (bbox[3] - bbox[1]) > height)
    {
    this->Script("grid %s -row 0 -column 1 -sticky news", 
                 this->ScrollBar->GetWidgetName());
    }
  else
    {
    this->Script("grid remove %s", this->ScrollBar->GetWidgetName());
    }
  // You don't want to stick the visible part right at the border of the
  // canvas, but let some space (2 pixels on top and left)
  this->Script("%s configure -scrollregion \"%d %d %d %d\"", 
               this->FileList->GetWidgetName(), 
               bbox[0] - 2, bbox[1] - 2, bbox[2], bbox[3]);
  //this->PostChildUpdate();
}

//-----------------------------------------------------------------------------
void vtkPVServerFileDialog::CalculateBBox(vtkKWWidget* canvas, 
                                          const char* name, 
                                          int bbox[4])
{
  const char *result;
  // Get the bounding box for the name. We may need to highlight it.
  result = this->Script("%s bbox %s", canvas->GetWidgetName(), name);
  if(sscanf(result, "%d %d %d %d", bbox, bbox+1, bbox+2, bbox+3) < 4)
    {
    // There are no objects in the canvas.  Create a fake bounding
    // box.
    bbox[0] = 0;
    bbox[1] = 0;
    bbox[2] = 1;
    bbox[3] = 1;
    }
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::AddDescriptionString(const char* str)
{
  this->FileTypeDescriptions->AddString(str);
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::AddExtensionString(const char* str)
{
  this->FileTypeStrings->AddString(str);
}

//----------------------------------------------------------------------------
void vtkPVServerFileDialog::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);  
}
