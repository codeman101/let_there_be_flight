@replaceMethod(VehicleEventsTransition)
protected final func HandleCameraInput(const stateContext: ref<StateContext>, scriptInterface: ref<StateGameScriptInterface>) -> Void {
  // 2.5: the game's body (toggle on release, cinematic camera on hold), with LTBF's rule that no camera change happens while flying.
  if scriptInterface.IsActionJustReleased(n"ToggleVehCamera") && !this.IsVehicleCameraChangeBlocked(scriptInterface) && !FlightController.GetInstance().active {
    this.RequestToggleVehicleCamera(scriptInterface);
  };
  if scriptInterface.IsActionJustHeld(n"HoldCinematicCamera") && !this.IsInItemWheelState(stateContext) && !this.IsVehicleCameraChangeBlocked(scriptInterface) && !FlightController.GetInstance().active {
    this.RequestVehicleCinematicCamera(scriptInterface);
  };
}

// @wrapMethod(vehicleVisualCustomizationHotkeyController)
// protected cb func OnAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool {
//   let isVehicleCustomizationAvailable: Bool = this.m_player.GetMountedVehicle().GetVehicleComponent().GetIsVehicleVisualCustomizationEnabled();
//   if Equals(ListenerAction.GetName(action), n"VehicleVisualCustomization") {
//     if VehicleSystem.IsPlayerInVehicle(this.GetPlayer().GetGame()) && isVehicleCustomizationAvailable {
//       return true;
//     };
//     if Equals(ListenerAction.GetType(action), gameinputActionType.BUTTON_HOLD_COMPLETE) {
//       if IsDefined(this.m_animationProxy) && this.m_animationProxy.IsPlaying() {
//         this.m_animationProxy.GotoEndAndStop(true);
//         this.m_animationProxy = null;
//       };
//       this.m_animationProxy = this.PlayLibraryAnimation(n"onFailUse_carMod");
//     };
//   };
// }

// not the right place
// @replaceMethod(VehicleTransition)
// protected final func ResetVehicleCamera(scriptInterface: ref<StateGameScriptInterface>) -> Void {
//   // let camEvent: ref<vehicleCameraResetEvent> = new vehicleCameraResetEvent();
//   // scriptInterface.executionOwner.QueueEvent(camEvent);
//   return;
// }
