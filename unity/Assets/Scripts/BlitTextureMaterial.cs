using UnityEngine;
using System;

[RequireComponent(typeof(Camera))]
[AddComponentMenu("Image Effects/Blit Texture Material")]
public class BlitTextureMaterial : MonoBehaviour 
{
    public Shader shader;
    public Material texMaterial = null;
    public float fixAspectRatio = 1;
    public float gammaCorrection = 2.2f;


    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        if (texMaterial.mainTexture)
        {
            material.SetFloat("_GammaCorrection", gammaCorrection);
            material.SetFloat("_FixAspect", fixAspectRatio);
            material.SetTexture("_MainTex", texMaterial.mainTexture);
            Graphics.Blit(texMaterial.mainTexture, destination, material);
        }
        else
        {
            Graphics.Blit(source, destination, material);   
        }

        RenderTexture.active = destination;
    }

    void OnEnable()
    {
        // Try to find the shader
        if (!shader)
            shader = Shader.Find("DTPD/BlitTextureMaterial");
    }

    void Start()
    {
        if (!texMaterial)
        {
            Debug.LogError("Missing reference to a material with texture.");
            enabled = false;
            return;
        }

        if (!shader)
            enabled = false;
    }

    protected virtual void OnDisable()
    {
        if (m_Material)
        {
            DestroyImmediate(m_Material);
        }
    }

    private Material m_Material;
    protected Material material
    {
        get
        {
            if (m_Material == null)
            {
                m_Material = new Material(shader);
                m_Material.hideFlags = HideFlags.HideAndDontSave;
            }
            return m_Material;
        }
    }
}
