# CreateTwinmotionUI.py
# Unreal Engine Python script to create Twinmotion-style UI Widget Blueprints
# Run this script from Unreal Editor using: Tools > Execute Python Script

import unreal

def create_widget_blueprint(asset_path, asset_name):
    """Create a new Widget Blueprint asset"""
    factory = unreal.WidgetBlueprintFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    blueprint = asset_tools.create_asset(asset_name, asset_path, unreal.WidgetBlueprint, factory)

    if blueprint:
        unreal.log(f"Created Widget Blueprint: {asset_path}/{asset_name}")
        return blueprint
    else:
        unreal.log_error(f"Failed to create Widget Blueprint: {asset_path}/{asset_name}")
        return None

def main():
    """Main function to create all Twinmotion UI widgets"""

    base_path = "/Game/BluePrint/UI"
    widgets_path = f"{base_path}/Widgets"
    panels_path = f"{widgets_path}/Panels"

    # Create directory structure if needed
    editor_asset_lib = unreal.EditorAssetLibrary()

    # Create main widgets
    widgets_to_create = [
        (base_path, "WBP_TwinmotionUI"),
        (widgets_path, "WBP_ThumbnailItem"),
        (panels_path, "WBP_TopBar"),
        (panels_path, "WBP_LeftPanel"),
        (panels_path, "WBP_RightPanel"),
        (panels_path, "WBP_BottomPanel"),
    ]

    created_widgets = []

    for path, name in widgets_to_create:
        widget = create_widget_blueprint(path, name)
        if widget:
            created_widgets.append((path, name, widget))

    # Save all created assets
    unreal.EditorAssetLibrary.save_directory(base_path)

    unreal.log("=== Twinmotion UI Widget Blueprints Created ===")
    for path, name, widget in created_widgets:
        unreal.log(f"  - {path}/{name}")

    unreal.log("\nNext steps:")
    unreal.log("1. Open each widget blueprint in the UMG Designer")
    unreal.log("2. Design the UI layout according to Twinmotion specifications")
    unreal.log("3. Use the provided C++ TwinmotionUIManager for state management")

if __name__ == "__main__":
    main()
