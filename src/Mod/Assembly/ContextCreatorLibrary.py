import FreeCAD as App

from PySide.QtCore import QT_TRANSLATE_NOOP

if App.GuiUp:
    import FreeCADGui as Gui
    from PySide import QtCore, QtGui, QtWidgets

transparency_level = 75

import UtilsAssembly
import Preferences

class ContextCreationSystem:
    COPYABLE_OBJECT_TYPES = ["Body", "Part", "LinkedObject", "DocumentObject", "Feature"]

    def __init__(self, editPart, assembly):
        pass

    def getObjFromRefString(self, ref):
        if "." in ref:
            splitRef = ref.split(".")
            print(splitRef[0])
            document = splitRef[0]
            splitRef.pop(0)
            refObj = splitRef
            refObj = ".".join(splitRef)
            print(document)
            print(refObj)
            document = App.getDocument(document.replace("-", "_"))
            return document.getObject(refObj)

    def getLinkedDoc(self, obj):
        document = None
        
        if hasattr(obj, "LinkedObject"):
            document = obj.LinkedObject.Document
        else:
            for child in obj.OutList:
                if hasattr(child, "LinkedObject"):
                    document = child.LinkedObject.Document
                    continue
                    
        return document

    def getLinkedObj(self, obj):
        object = None
        
        if hasattr(obj, "LinkedObject"):
            obj.LinkTransform = False
            object = obj.LinkedObject
        else:
            for child in obj.OutList:
                if hasattr(child, "LinkedObject"):
                    child.LinkTransform = False
                    object = child.LinkedObject
                    continue
                    
        return object

    def getCopyableObjectsInAssembly(self, assembly):
        objects = []
        
        for obj in assembly.OutList:
            if type(obj).__name__ in self.COPYABLE_OBJECT_TYPES and "Placement" in obj.PropertiesList:
                objects.append(obj)
        return objects


    def addOffsets(self, objects,linked_obj, linked_placement):
        linked_obj.Placement = linked_placement

    def createGroup(self, objects, target_document, linked_obj, edit_selection):
        if App.ActiveDocument.getObjectsByLabel("AssemblyContext"):
            App.ActiveDocument.removeObject(App.ActiveDocument.getObjectsByLabel("AssemblyContext")[0].Name)

        group = App.ActiveDocument.addObject("App::DocumentObjectGroup", "Group")
        group.addProperty("App::PropertyString", "EditedPart")
        group.EditedPart = linked_obj.Name
        group.addProperty("App::PropertyString", "EditedPartLink")
        group.EditedPartLink = App.ActiveDocument.Name + "." + edit_selection.Name
        
        for obj in objects:
            group.addObject(obj)
        group.Label = "AssemblyContext"
        self.move_group_to_doc(target_document, group, True)

    def _removeChildren(self, obj):
        for child in obj.OutList:
            App.ActiveDocument.removeObject(child.Name)

    '''
    def addProperties(objects, copiedObjects):
        objectNames = []
        for obj in objects:
            objectsNames.append(obj.Label)
        for copiedObj in copiedObjects:
            if copiedObj.Label in objectNames:
                propertyString = App.ActiveDocument.Name + "." + copiedObj.Name
                copiedObj.addProperty("App::PropertyString")
    '''

    def createCopy(self, objects, edit_obj):
        copiedObjects = []
        for obj in objects:
            if(obj is edit_obj):
                continue
            propertyString = App.ActiveDocument.Name + "." + obj.Name
            #copiedObj = App.ActiveDocument.copyObject(obj, True)
            copiedObj = App.ActiveDocument.addObject("Part::Feature", obj.Label)
            copiedObj.Shape = obj.Shape
            copiedObj.ViewObject.Transparency = transparency_level
            copiedObj.addProperty("App::PropertyString", "RefObj")
            copiedObj.RefObj = propertyString
            copiedObjects.append(copiedObj)
            
            
        return copiedObjects

    def createContext(self, linkedObj, target_document, normalObjects, linked_placement, edit_selection):
        Gui.setActiveDocument(target_document.Name)
        objects = self.createCopy(normalObjects, edit_selection)
        self.addOffsets(objects, linkedObj, linked_placement)
        self.createGroup(objects, target_document, linkedObj, edit_selection)

    def _copy_selection_test(self):
        objects = App.ActiveDocument.copyObject(Gui.Selection.getSelection(), True)
        self.createGroup(objects)
        
    def updateShape(self, obj, refObj):
        obj.Shape = refObj.Shape
        obj.recompute(True)
        App.ActiveDocument=App.ActiveDocument
        documentFileName = App.ActiveDocument.FileName
        
    def move_group_to_doc(self, dest_doc, group, deleteOriginal):
        # Get the source and destination documents
        source_doc = App.ActiveDocument

        # Copy the object to the destination document
        new_obj = dest_doc.copyObject(group, True)

        # Delete the original object from the source document
        # source_doc.removeObject(obj.Name)
        if deleteOriginal:
            for obj in group.OutList:
                if obj.OutList:
                    self.removeChildren(obj)
                source_doc.removeObject(obj.Name)
            source_doc.removeObject(group.Name)

    def getPartParent(self, obj):
        final_obj = None
        
        if type(obj).__name__ == "Part":
            return obj
        
        for parent in obj.InList:
            if(type(parent).__name__ == "Part"):
                final_obj = parent
            else:
                print(type(parent).__name__)
        return final_obj


        def createGridLayout(self):
            self.horizontalGroupBox = QtGui.QGroupBox("Assembly Context Creator")
            self.layout = QtGui.QGridLayout()
            
            label = QtGui.QLabel(self.horizontalGroupBox)
            label.setText("In Assembly 3, select the 'Parts' object,\n In the default assembly wb, select the 'Assembly' Object")

            self.updateButton = QtGui.QPushButton(self.horizontalGroupBox)
            self.updateButton.setText("Update Selected Context")
            self.updateButton.clicked.connect(self.UpdateContext)
            self.PB_01= QtGui.QPushButton(self.horizontalGroupBox)
            self.PB_01.setText("Select the Assembly Part")
            self.PB_01.clicked.connect(self.SelectPartButton) # slot: "PB 01"
            self.layout.addWidget(self.PB_01,1,0)
            self.layout.addWidget(self.updateButton,2,0)
            self.layout.addWidget(label, 0,0)
            
            self.horizontalGroupBox.setLayout(self.layout)

    def UpdateContext(self):
        selection = Gui.Selection.getSelection()[0]
        if "EditedPart" in selection.PropertiesList:
            self.editedPart = App.ActiveDocument.getObject(selection.EditedPart)
            self.objectToUpdate = self.getCopyableObjectsInAssembly(selection)
                
            EditedPartLink = self.getObjFromRefString(selection.EditedPartLink)
            self.editedPart.Placement = EditedPartLink.Placement
                
            for obj in self.objectToUpdate:
                if "RefObj" in obj.PropertiesList:
                    refObj = self.getObjFromRefString(obj.RefObj)
                    assemblyDocument = refObj.Document
                        
                    try:
                        obj.Placement = refObj.Placement
                        #obj.Shape = Part.Shape()
                        self.updateShape(obj, refObj)
                        App.ActiveDocument.recompute()
                    except:
                        print("could not update object: " + refObj.Label)
            App.ActiveDocument.recompute()
            documentFileName = App.ActiveDocument.FileName
            App.ActiveDocument.save()
            App.closeDocument(App.ActiveDocument.Name)
            App.openDocument(documentFileName)
        else:
            App.Console.PrintWarning("The selection is not an assembly context!")

        def SelectPartButton(self): # slot: PushButton
            ''' Push Button 01 clicked  '''
            print('Select Button Pressed')
            
            selection = Gui.Selection.getSelection()
            if selection:
                if self.current_selection == 1:
                    self.objects = getCopyableObjectsInSelection()
                    if len(self.objects) == 0:
                        print("No suitable objects could be found. The suitable object types are: ", COPYABLE_OBJECT_TYPES, "\n")
                        print("To add a datatype, change the 'COPYABLE_OBJECT_TYPES' variable in this macro.")
                        print(self.objects)
                    else:
                        self.PB_01.setText("Select the Part to Edit")
                        self.layout.removeWidget(self.updateButton)
                        self.updateButton = None
                        self.current_selection = 2
                else:
                    self.edit_selection = selection[0]
                    print(self.edit_selection.Name)
                    
                    #self.first_selection = getPartParent(self.first_selection)
                    
                    if(self.edit_selection is None):
                        print("You must select a part")
                    else:
                        if self.edit_selection in self.objects:
                            print("in objects")
                            self.objects.remove(self.edit_selection)
                        linked_placement = self.edit_selection.Placement
                        target_document = getLinkedDoc(self.edit_selection)
                        linked_object = getLinkedObj(self.edit_selection)

                        print(linked_object.Placement)
                        
                        if target_document is None:
                            print("Could not locate the document of the original part.\n The selected part must be a link or have a link as a child")
                        else:
                            createContext(linked_object, target_document, self.objects, linked_placement, self.edit_selection)
                    
                    Gui.Control.closeDialog()
            else:
                print("Nothing is selected!")