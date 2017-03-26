import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property string diffuseColor: "red"
    property string bump: "no_bumps"
    property string specular: ""

    property real x: 0
    property real y: 0
    property real z: 0
    property alias shininess: material.shininess
    property real scale: 1.0

    RenderableEntity {
        id: barrel
        source: "metalbarrel/metal_barrel.obj"
        scale: 0.03 * root.scale
        position: Qt.vector3d(root.x, root.y, root.z)

        material: NormalDiffuseSpecularMapMaterial {
            id: material
            diffuse: "metalbarrel/diffus_" + root.diffuseColor + ".webp"
            normal: "metalbarrel/normal_" + root.bump + ".webp"
            specular: {
                if (root.specular !== "" )
                    return "metalbarrel/specular_" + root.specular + ".webp"
                else
                    return "metalbarrel/specular.webp"
            }

            shininess: 10.0
        }
    }
}
