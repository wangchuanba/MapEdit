import Qt3D.Core 2.0
import Qt3D.Render 2.0

TechniqueFilter {
    id: root
    objectName : "techniqueFilter"

    // Expose camera to allow user to choose which camera to use for rendering
    property alias camera: cameraSelector.camera
    property alias window: surfaceSelector.surface

    // Select the forward rendering Technique of any used Effect
    matchAll: [ FilterKey { name: "renderingStyle"; value: "forward" } ]

    RenderSurfaceSelector {
        id: surfaceSelector

        // Use the whole viewport
        Viewport {
            id: viewport
            objectName : "viewport"
            normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

            // Use the specified camera
            CameraSelector {
                id : cameraSelector
                objectName : "cameraSelector"
                ClearBuffers {
                    buffers : ClearBuffers.ColorDepthBuffer
                    clearColor: "green"
                    SortPolicy {
                        sortTypes: [
                            SortPolicy.StateChangeCost,
                            SortPolicy.Material
                        ]
                    }
                }
            }
        }
    }
}
