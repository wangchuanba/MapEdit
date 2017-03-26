import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    objectName: "root"

    components: [
        RenderSettings {
            activeFrameGraph: SortedForwardRenderer {
                id: renderer
                camera: mainCamera
            }
        },
        InputSettings { }
    ]


    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( 0.0, 3.5, 25.0 )
        viewCenter: Qt.vector3d( 0.0, 3.5, 0.0 )
    }

    FirstPersonCameraController { camera: mainCamera }

    PhongMaterial {
        id: redAdsMaterial
        ambient: Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
        diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )
    }

    PlaneEntity {
        id: floor

        width: 100
        height: 100
        resolution: Qt.size(20, 20)

        material: NormalDiffuseSpecularMapMaterial {
            ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
            diffuse:  "textures/diffuse.webp"
            specular: "textures/specular.webp"
            normal:   "textures/normal.webp"
            textureScale: 10.0
            shininess: 80.0
        }
    }

    Barrel {
        id: barry1
        x: -4
    }

    Barrel {
        id: barry2
        x: barry1.x + 2
        diffuseColor: "rust"
        bump: "hard_bumps"
        specular: "rust"
    }

    Barrel {
        id: barry3
        x: barry1.x + 4
        diffuseColor: "blue"
        bump: "middle_bumps"
    }

    Barrel {
        id: barry4
        x: barry1.x + 6
        diffuseColor: "green"
        bump: "soft_bumps"
    }

    Barrel {
        id: barry5
        x: barry1.x + 8
        diffuseColor: "stainless_steel"
        bump: "no_bumps"
        specular: "stainless_steel"
        shininess: 150
    }
}
