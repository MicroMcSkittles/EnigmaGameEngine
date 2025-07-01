 #include "Engine/Render2DProc.h"
 #include "Engine/Engine.h"
 #include "Core/Process/Application.h"

 #include "Engine/ECS/EntityComponentSystem.h"
 #include "Engine/ECS/Component.h"
 #include "Engine/ECS/RenderComponent.h"

 namespace Enigma {
 	namespace Engine {

 		void Render2DProc::StartUp()
 		{
 			Surface& surface = GetEngine()->GetSurface();
 			Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(surface);
 			m_CurrentCamera = new Renderer::OrthographicCamera(viewBox);
 			m_LastFrameSize = surface.scale;
 		}

 		void Render2DProc::Render()
 		{
 			Surface& surface = GetEngine()->GetSurface();
 			if (m_LastFrameSize != surface.scale) {
 				m_LastFrameSize = surface.scale;
 				Renderer::Render2D::Resize(surface.scale.x, surface.scale.y);
 			}

 			Renderer::Render2D::StartFrame(m_CurrentCamera);

            for (ECS::Render2D& renderComp : ECS::ECS::GetPool<ECS::Render2D>()->GetData()) {
                ECS::Transform& transform = ECS::ECS::GetComponent<ECS::Transform>(renderComp.parentEntity);
                
                if (renderComp.texture == nullptr) {
                    Renderer::Render2D::DrawQuad(
                        transform.position,
                        transform.scale,
                        transform.rotation.z,
                        renderComp.depth,
                        renderComp.tint
                    );
                }
                else {
                    Renderer::Render2D::DrawQuad(
                        transform.position,
                        transform.scale,
                        transform.rotation.z,
                        renderComp.depth,
                        renderComp.texture,
                        renderComp.tint
                    );
                }
            }

 			Renderer::Render2D::EndFrame();
 		}
 	}
 }