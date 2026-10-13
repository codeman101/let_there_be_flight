
#include "Flight/Component.hpp"
#include "Utils/PlatformLayout.hpp"
#include "LoadResRef.hpp"
#include "Utils/FlightModule.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/Generated/WidgetHudComponent.hpp>
#include <RED4ext/Scripting/Natives/Generated/ink/HudEntriesResource.hpp>
#include <Extensions/MeshComponent.hpp>

#ifdef __APPLE__
#include <cstdlib>
#include <cstring>
// LTBF R42e (macOS, diagnostics): LTBF_MAC_SKIP=<comma list> leaves parts of the per-vehicle setup out, to find which one breaks the
// entity teardown (Boot C: SIGSEGV in the game's Entity_Uninitialize after a teleport). Words: thrusters (the thruster mesh components
// through EntityExt::AddComponent), config (the flight configuration), hud (the FlightHUD WidgetHudComponent), component (the
// FlightComponent itself, and so everything). Unset = Windows behaviour.
static bool LtbfMacSkip(const char *aWhat) {
  static const char *s_skip = std::getenv("LTBF_MAC_SKIP");
  static bool s_logged = false;
  if (!s_logged) {
    s_logged = true;
    spdlog::info("[ltbf] LTBF_MAC_SKIP={}", s_skip ? s_skip : "(unset)");
  }
  return s_skip && std::strstr(s_skip, aWhat) != nullptr;
}
#define LTBF_MAC_SKIP(what) LtbfMacSkip(what)
#else
#define LTBF_MAC_SKIP(what) false
#endif

/// @hash 3490519617
// void Entity_InitializeComponents(RED4ext::ent::Entity *entity, void *a2, void *a3);

// void ent::Entity::Initialize(job::Builder &, ent::EntitySetupContext const &)
REGISTER_FLIGHT_HOOK_HASH(void, 3490519617, Entity_InitializeComponents, EntityExt *entity, void *a2, void *a3) {
  auto rtti = RED4ext::CRTTISystem::Get();

  auto isVehicle = entity->IsOfClass(rtti->GetClass("vehicleBaseObject"));

  auto vehicle = reinterpret_cast<RED4ext::vehicle::BaseObject *>(entity);

  auto canEnterFlight_func = rtti->GetClass("vehicleBaseObject")->GetFunction("CanEnterFlight");
  bool canEnterFlight = false;
  if (isVehicle && canEnterFlight_func) {
    RED4ext::ExecuteFunction(vehicle, canEnterFlight_func, &canEnterFlight);
  }

  if (isVehicle && canEnterFlight && !LTBF_MAC_SKIP("component")) {
    auto fc = (FlightComponent *)FlightComponent::GetRTTIType()->CreateInstance(true);
    fc->name = "flightComponent";
    fc->id = RED4ext::CRUID::Next();
    // fc->entity = entity;

    auto fch = RED4ext::Handle<FlightComponent>(fc);
    vehicle->componentsStorage.components.EmplaceBack(LTBF_AS_COMPONENT_HANDLE(fch));

    // auto fc = (FlightComponent*)FlightComponent::GetRTTIType()->CreateInstance(true);
    // fc->name = "flightComponent";
    // auto h = RED4ext::Handle<FlightComponent>(fc);
    // h.refCount->IncRef();
    // h.refCount->IncRef();
    // entity->componentsStorage.components.EmplaceBack(h);

    // auto const vcc = entity->GetComponent<RED4ext::ent::VisualControllerComponent>();
    // auto chassis = entity->GetComponent<RED4ext::vehicle::ChassisComponent>();
    auto const vs = entity->GetComponent<RED4ext::ent::SlotComponent>("vehicle_slots");
    // auto osc = entity->GetComponent<RED4ext::game::OccupantSlotComponent>();

    if (vs) {
      // {
        // auto slot = reinterpret_cast<RED4ext::ent::Slot *>(rtti->GetClass("entSlot")->CreateInstance(true));
        // slot->boneName = "roof_border_front";
        // slot->slotName = "roof_border_front";
        // vs->slots.EmplaceBack(*slot);
        // vs->slotIndexLookup.Emplace(slot->slotName, vs->slots.size - 1);
      // }
      // FlightWeapons::AddWeaponSlots(vs);

      auto configurationCls = IFlightConfiguration::GetConfigurationClass(entity);

      if (configurationCls && !LTBF_MAC_SKIP("config")) {
        // spdlog::info("Looked for class '{}' using '{}'", className, configurationCls->name.ToString());
        auto configuration = configurationCls->CreateInstance<IFlightConfiguration *>(true);
        configurationCls->ConstructCls(configuration);

        // auto handle = RED4ext::Handle<IFlightConfiguration>(configuration);
        // configuration->ref = RED4ext::WeakHandle(*reinterpret_cast<RED4ext::Handle<RED4ext::ISerializable> *>(&handle));
        configuration->nativeType = configurationCls;
        configuration->component = RED4ext::WeakHandle<FlightComponent>(fch);
        configuration->component.refCount->IncRef();
        fc->configuration = RED4ext::Handle<IFlightConfiguration>(configuration);
        // fc->configuration.refCount->IncRef();

        configuration->Setup(vehicle);
        // configuration->AddMeshes(entity, vcc);
        for (auto const & thruster : configuration->thrusters) {
          if (thruster->meshComponent && !LTBF_MAC_SKIP("thrusters")) {
            thruster->meshComponent.refCount->IncRef();
            entity->AddComponent(thruster->meshComponent);
          }
        }
        configuration->AddSlots(vs);
      } else {
        // spdlog::info("Looked for class '{}'", className);
      }
    }

    // UI
    //{
    //  // MeshComponent
    //  auto mc = (RED4ext::ent::MeshComponent *)rtti->GetClass("entMeshComponent")->CreateInstance(true);

    //  RED4ext::CName mesh = "user\\jackhumbert\\meshes\\flight_ui.mesh";
    //  mc->mesh.ref = mesh;
    //  mc->name = "flight_screen";
    //  mc->meshAppearance = "default";

    //  auto mcHandle = RED4ext::Handle<RED4ext::ent::MeshComponent>(mc);
    //  entity->componentsStorage.components.EmplaceBack(mcHandle);
    //}
    //{
    //  // WorldWidgetComponent
    //  auto wwc = (RED4ext::WorldWidgetComponent *)rtti->GetClass("WorldWidgetComponent")->CreateInstance(true);

    //  wwc->name = "flight_ui";

    //  RED4ext::CName fc = "user\\jackhumbert\\widgets\\flight_ui.inkwidget";
    //  wwc->widgetResource.ref = fc;

    //  auto mtb = (RED4ext::world::ui::MeshTargetBinding
    //  *)rtti->GetClass("worlduiMeshTargetBinding")->CreateInstance(true); mtb->bindName = "flight_screen";
    //  wwc->meshTargetBinding = RED4ext::Handle<RED4ext::world::ui::MeshTargetBinding>(mtb);

    //  auto wwcHandle = RED4ext::Handle<RED4ext::WorldWidgetComponent>(wwc);
    //  entity->componentsStorage.components.EmplaceBack(wwcHandle);
    //}

    // UI Info Panel
    //{
    //  // MeshComponent
    //  auto mc = (RED4ext::ent::MeshComponent *)rtti->GetClass("entMeshComponent")->CreateInstance(true);

    //  RED4ext::CName mesh = "user\\jackhumbert\\meshes\\flight_ui_info.mesh";
    //  mc->mesh.ref = mesh;
    //  mc->name = "flight_screen_info";
    //  mc->meshAppearance = "screen_ui";
    //  mc->renderingPlane = RED4ext::ERenderingPlane::RPl_Weapon;
    //  mc->forcedLodDistance = RED4ext::ent::ForcedLodDistance::VehicleInterior;

    //  entity->componentsStorage.components.EmplaceBack(RED4ext::Handle<RED4ext::ent::MeshComponent>(mc));
    //}
    //{
    //  // WorldWidgetComponent
    //  auto wwc = (RED4ext::WorldWidgetComponent *)rtti->GetClass("WorldWidgetComponent")->CreateInstance(true);

    //  wwc->name = "flight_ui_info";

    //  RED4ext::CName fc = "user\\jackhumbert\\widgets\\flight_ui.inkwidget";
    //  wwc->widgetResource.ref = fc;
    //  wwc->spawnDistanceOverride = 20.0;
    //  wwc->sceneWidgetProperties.renderingPlane = RED4ext::ERenderingPlane::RPl_Weapon;
    //  auto mtb = (RED4ext::world::ui::MeshTargetBinding
    //  *)rtti->GetClass("worlduiMeshTargetBinding")->CreateInstance(true); mtb->bindName = "flight_screen_info";
    //  wwc->meshTargetBinding = RED4ext::Handle<RED4ext::world::ui::MeshTargetBinding>(mtb);

    //  entity->componentsStorage.components.EmplaceBack(RED4ext::Handle<RED4ext::WorldWidgetComponent>(wwc));
    //}
    //{
    //  auto gpsp =
    //      (RED4ext::game::projectile::SpawnComponent
    //      *)rtti->GetClass("gameprojectileSpawnComponent")->CreateInstance(true);
    //  gpsp->name = "projectileSpawn8722";
    //  gpsp->projectileTemplates.EmplaceBack("exploding_bullet");
    //  auto htb = (RED4ext::ent::HardTransformBinding *)rtti->GetClass("entHardTransformBinding")->CreateInstance(true);
    //  htb->bindName = "vehicle_slots";
    //  htb->slotName = "ThrusterFL";
    //  gpsp->parentTransform = RED4ext::Handle<RED4ext::ent::ITransformBinding>(htb);
    //  entity->componentsStorage.components.EmplaceBack(RED4ext::Handle<RED4ext::game::projectile::SpawnComponent>(gpsp));
    //}

    if (!LTBF_MAC_SKIP("hud")) {
      auto whc = rtti->GetClass("WidgetHudComponent")->CreateInstance<RED4ext::WidgetHudComponent *>(true);
      whc->name = "FlightHUD";
      whc->id = RED4ext::CRUID::Next();
      // auto resource =
      // RED4ext::Ref<RED4ext::ink::HudEntriesResource>("user\\jackhumbert\\widgets\\hud_flight.inkhud", true);
      whc->hudEntriesResource.path = "user\\jackhumbert\\widgets\\hud_flight.inkhud";
      // whc->hudEntriesResource.token = resource.token;
      LoadResRef<RED4ext::ink::HudEntriesResource>(&whc->hudEntriesResource.path, &whc->hudEntriesResource.token, false);
      entity->componentsStorage.components.EmplaceBack(LTBF_AS_COMPONENT_HANDLE(RED4ext::Handle<RED4ext::WidgetHudComponent>(whc)));
    }

    //{
    //  auto gcc = (RED4ext::game::CameraComponent *)rtti->GetClass("gameCameraComponent")->CreateInstance(true);
    //  gcc->name = "frontCamera";
    //  auto htb = (RED4ext::ent::HardTransformBinding *)rtti->GetClass("entHardTransformBinding")->CreateInstance(true);
    //  htb->bindName = "vehicle_slots";
    //  htb->slotName = "roof_border_front";
    //  gcc->parentTransform = RED4ext::Handle<RED4ext::ent::ITransformBinding>(htb);
    //  entity->componentsStorage.components.EmplaceBack(RED4ext::Handle<RED4ext::game::CameraComponent>(gcc));
    //}
  }

  Entity_InitializeComponents_Original(entity, a2, a3);
}