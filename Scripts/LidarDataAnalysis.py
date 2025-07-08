import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import json
from datetime import datetime

# Configuración del análisis
BACKGROUND_THRESHOLD = 90  # cm - distancias > 90cm se consideran fondo
MAX_DISTANCE_THRESHOLD = 150  # cm - distancias > 150cm se descartan (pared)

# Descripción de cada medición (simplificada)
MEASUREMENT_DESCRIPTIONS = {
    1: "Libro horizontal (26x22cm) a 60cm del centro",
    2: "Libro vertical (26x22cm) a 60cm del centro",
    3: "Gato negro (13x8cm) a 40cm izquierda, Esfinge (23x3cm) a 70cm derecha",
    4: "Posiciones intercambiadas: Esfinge izquierda, Gato derecha",
    5: "Dos tazas a 60cm separadas 5cm",
    6: "Jarrón (10x10cm) a 70cm con flor (35cm alto) + pelota izquierda",
    7: "Cuadro grande (50x30cm) a 65cm ocupando gran parte del FOV",
    8: "Múltiples figuras pequeñas: fila a 35cm (pequeñas) y fila a 75cm (altas)"
}

def analizar_medicion_simple(archivo_lidar, measurement_id):
    """
    Análisis simplificado de una medición LiDAR
    """
    print(f"\nAnalizando Medición {measurement_id}: {archivo_lidar}")
    print(f"Setup: {MEASUREMENT_DESCRIPTIONS.get(measurement_id, 'N/A')}")
    
    # Cargar datos
    try:
        datos_lidar = np.loadtxt(archivo_lidar, usecols=(0, 1, 2))
    except Exception as e:
        print(f"Error cargando {archivo_lidar}: {e}")
        return None
    
    # Conversión a coordenadas cartesianas
    angulosH = np.radians(datos_lidar[:, 0])
    angulosV = np.radians(datos_lidar[:, 1])
    distancias = datos_lidar[:, 2]
    
    # Filtrar mediciones inválidas (distancia 0) y muy lejanas (> 150cm)
    mask_valid = (distancias > 0) & (distancias <= MAX_DISTANCE_THRESHOLD)
    angulosH = angulosH[mask_valid]
    angulosV = angulosV[mask_valid]
    distancias = distancias[mask_valid]
    
    print(f"   ✓ Puntos válidos después de filtrado: {len(distancias)} (filtrados: distancia > {MAX_DISTANCE_THRESHOLD}cm)")
    
    # Coordenadas cartesianas
    x = distancias * np.sin(angulosV) * np.cos(angulosH)
    y = distancias * np.sin(angulosV) * np.sin(angulosH)
    z = distancias * np.cos(angulosV)
    
    # Separar objeto y fondo basado en distancia
    mask_objeto = distancias < BACKGROUND_THRESHOLD
    mask_fondo = distancias >= BACKGROUND_THRESHOLD
    
    # Estadísticas
    puntos_objeto = np.sum(mask_objeto)
    puntos_fondo = np.sum(mask_fondo)
    
    distancia_media_objeto = np.mean(distancias[mask_objeto]) if puntos_objeto > 0 else 0
    distancia_media_fondo = np.mean(distancias[mask_fondo]) if puntos_fondo > 0 else 0
    
    print(f"   ✓ Puntos totales: {len(distancias)}")
    print(f"   ✓ Puntos objeto (<{BACKGROUND_THRESHOLD}cm): {puntos_objeto} ({puntos_objeto/len(distancias)*100:.1f}%)")
    print(f"   ✓ Puntos fondo (≥{BACKGROUND_THRESHOLD}cm): {puntos_fondo} ({puntos_fondo/len(distancias)*100:.1f}%)")
    print(f"   ✓ Distancia media objeto: {distancia_media_objeto:.1f} cm")
    print(f"   ✓ Distancia media fondo: {distancia_media_fondo:.1f} cm")
    
    # Crear visualización
    crear_visualizacion_simple(x, y, z, distancias, mask_objeto, mask_fondo, measurement_id)
    
    # Generar texto del análisis y guardarlo
    texto_analisis = f"""
ANÁLISIS DE ADQUISICIÓN LIDAR - MEDICIÓN {measurement_id}

Setup: {MEASUREMENT_DESCRIPTIONS.get(measurement_id, 'N/A')}

ESTADÍSTICAS DE ADQUISICIÓN:
• Total de puntos adquiridos: {len(distancias)}
• Rango de distancias: {np.min(distancias):.1f} - {np.max(distancias):.1f} cm
• Distancia media: {np.mean(distancias):.1f} ± {np.std(distancias):.1f} cm
• Filtrado aplicado: Descartadas distancias > {MAX_DISTANCE_THRESHOLD}cm

SEPARACIÓN OBJETO/FONDO (umbral {BACKGROUND_THRESHOLD}cm):
• Puntos objeto: {np.sum(mask_objeto)} ({np.sum(mask_objeto)/len(distancias)*100:.1f}%)
• Puntos fondo: {np.sum(mask_fondo)} ({np.sum(mask_fondo)/len(distancias)*100:.1f}%)
• Distancia media objeto: {np.mean(distancias[mask_objeto]) if np.sum(mask_objeto) > 0 else 0:.1f} cm
• Distancia media fondo: {np.mean(distancias[mask_fondo]) if np.sum(mask_fondo) > 0 else 0:.1f} cm

CORRESPONDENCIA CON SETUP:
• La vista frontal (Y-Z) es directamente comparable con la foto
• Puntos rojos representan objetos de interés
• Puntos azules representan el fondo del setup
• La distribución de distancias muestra la separación espacial
• Datos filtrados para eliminar reflexiones de pared (>{MAX_DISTANCE_THRESHOLD}cm)
"""
    
    # Guardar texto del análisis
    with open(f'Analisis_Medicion_{measurement_id}.txt', 'w', encoding='utf-8') as f:
        f.write(texto_analisis)
    print(f"  Guardado texto: Analisis_Medicion_{measurement_id}.txt")
    
    # Resultados para el reporte
    resultado = {
        'measurement_id': measurement_id,
        'description': MEASUREMENT_DESCRIPTIONS.get(measurement_id, 'N/A'),
        'total_points': len(distancias),
        'object_points': int(puntos_objeto),
        'background_points': int(puntos_fondo),
        'object_percentage': float(puntos_objeto/len(distancias)*100),
        'background_percentage': float(puntos_fondo/len(distancias)*100),
        'mean_distance_object': float(distancia_media_objeto),
        'mean_distance_background': float(distancia_media_fondo),
        'min_distance': float(np.min(distancias)),
        'max_distance': float(np.max(distancias))
    }
    
    return resultado

def crear_visualizacion_simple(x, y, z, distancias, mask_objeto, mask_fondo, measurement_id):
    """
    Crear visualización simple con nube de puntos, vista frontal y distribución de distancias
    """
    # Verificar si existe la imagen de cámara
    imagen_camara = f"CamaraSample_{measurement_id}.jpg"
    has_image = os.path.exists(imagen_camara)
    
    # Configurar layout
    if has_image:
        fig = plt.figure(figsize=(20, 12))
        gs = fig.add_gridspec(2, 3, hspace=0.3, wspace=0.3)
    else:
        fig = plt.figure(figsize=(16, 8))
        gs = fig.add_gridspec(1, 3, hspace=0.3, wspace=0.3)
    
    # 1. Nube de puntos 3D
    ax1 = fig.add_subplot(gs[0, 0], projection='3d')
    
    # Plotear objeto y fondo con colores diferentes
    if np.sum(mask_objeto) > 0:
        ax1.scatter(x[mask_objeto], y[mask_objeto], z[mask_objeto], 
                   c='red', s=15, alpha=0.8, label=f'Objeto ({np.sum(mask_objeto)} puntos)')
    
    if np.sum(mask_fondo) > 0:
        ax1.scatter(x[mask_fondo], y[mask_fondo], z[mask_fondo], 
                   c='lightblue', s=8, alpha=0.4, label=f'Fondo ({np.sum(mask_fondo)} puntos)')
    
    ax1.set_xlabel('X (cm)')
    ax1.set_ylabel('Y (cm)')
    ax1.set_zlabel('Z (cm)')
    ax1.set_title(f'Nube de Puntos 3D - Medición {measurement_id}')
    ax1.legend()
    
    # 2. Vista frontal (Y-Z) - comparable con la foto
    ax2 = fig.add_subplot(gs[0, 1])
    
    if np.sum(mask_objeto) > 0:
        ax2.scatter(y[mask_objeto], z[mask_objeto], 
                   c='red', s=20, alpha=0.8, label=f'Objeto')
    
    if np.sum(mask_fondo) > 0:
        ax2.scatter(y[mask_fondo], z[mask_fondo], 
                   c='lightblue', s=10, alpha=0.4, label=f'Fondo')
    
    ax2.set_xlabel('Y (cm)')
    ax2.set_ylabel('Z (cm)')
    ax2.set_title('Vista Frontal (Y-Z)\n')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.set_aspect('equal')
    
    # 3. Distribución de distancias
    ax3 = fig.add_subplot(gs[0, 2])
    
    # Histograma de todas las distancias
    ax3.hist(distancias, bins=30, alpha=0.7, color='gray', edgecolor='black', label='Todas las distancias')
    
    # Línea vertical para separar objeto/fondo
    ax3.axvline(x=BACKGROUND_THRESHOLD, color='red', linestyle='--', linewidth=2, 
                label=f'Umbral fondo ({BACKGROUND_THRESHOLD}cm)')
    
    # Estadísticas
    ax3.axvline(x=np.mean(distancias), color='blue', linestyle='-', alpha=0.7, 
                label=f'Media: {np.mean(distancias):.1f}cm')
    
    ax3.set_xlabel('Distancia (cm)')
    ax3.set_ylabel('Frecuencia')
    ax3.set_title('Distribución de Distancias')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. Imagen de cámara (si existe)
    if has_image:
        try:
            ax4 = fig.add_subplot(gs[1, 0])
            img = plt.imread(imagen_camara)
            ax4.imshow(img)
            ax4.set_title(f'Imagen del entorno de prueba - Medición {measurement_id}')
            ax4.axis('off')
        except Exception as e:
            print(f"⚠️ Error cargando imagen {imagen_camara}: {e}")
    
    # 5. Gráfico adicional o espacio libre (sin texto)
    if has_image:
        # Vista superior (X-Y) para aprovechar el espacio
        ax5 = fig.add_subplot(gs[1, 1])
        
        if np.sum(mask_objeto) > 0:
            ax5.scatter(x[mask_objeto], y[mask_objeto], 
                       c='red', s=15, alpha=0.8, label='Objeto')
        
        if np.sum(mask_fondo) > 0:
            ax5.scatter(x[mask_fondo], y[mask_fondo], 
                       c='lightblue', s=8, alpha=0.4, label='Fondo')
        
        ax5.set_xlabel('X (cm)')
        ax5.set_ylabel('Y (cm)')
        ax5.set_title('Vista Superior (X-Y)')
        ax5.legend()
        ax5.grid(True, alpha=0.3)
        ax5.set_aspect('equal')
        
        # Mapa de densidad de puntos
        ax6 = fig.add_subplot(gs[1, 2])
        
        # Crear histograma 2D de la vista frontal
        hist, xedges, yedges = np.histogram2d(y, z, bins=20)
        extent = [yedges[0], yedges[-1], xedges[0], xedges[-1]]
        
        im = ax6.imshow(hist, extent=extent, origin='lower', cmap='viridis', alpha=0.7)
        ax6.set_xlabel('Z (cm)')
        ax6.set_ylabel('Y (cm)')
        ax6.set_title('Densidad de Puntos\n(Vista Frontal)')
        plt.colorbar(im, ax=ax6, label='Número de puntos')
    
    plt.suptitle(f'Análisis de Adquisición LiDAR - Medición {measurement_id}\n{MEASUREMENT_DESCRIPTIONS.get(measurement_id, "N/A")}', 
                 fontsize=16, fontweight='bold')
    
    # Guardar
    plt.savefig(f'Adquisicion_Lidar_Medicion_{measurement_id}.png', dpi=300, bbox_inches='tight')
    print(f"   ✅ Guardado gráfico: Adquisicion_Lidar_Medicion_{measurement_id}.png")
    
    # Mostrar solo si es interactivo
    try:
        plt.show()
    except:
        pass
    
    plt.close()  # Cerrar figura para liberar memoria

def analisis_completo_simple():
    """
    Ejecutar análisis completo simplificado
    """
    print("🚀 ANÁLISIS SIMPLIFICADO DE ADQUISICIÓN LIDAR")
    print("=" * 60)
    
    # Buscar archivos LiDAR
    archivos_lidar = []
    for i in range(1, 9):
        archivo = f"LidarSample_{i}.txt"
        if os.path.exists(archivo):
            archivos_lidar.append((archivo, i))
    
    if not archivos_lidar:
        print("❌ No se encontraron archivos LiDAR")
        return
    
    print(f"📁 Archivos encontrados: {len(archivos_lidar)}")
    
    # Analizar cada medición
    resultados = []
    for archivo, measurement_id in archivos_lidar:
        resultado = analizar_medicion_simple(archivo, measurement_id)
        if resultado:
            resultados.append(resultado)
    
    # Crear reporte simple
    reporte = {
        'timestamp': datetime.now().isoformat(),
        'total_measurements': len(resultados),
        'background_threshold_cm': BACKGROUND_THRESHOLD,
        'measurements': resultados,
        'summary': {
            'total_points': sum(r['total_points'] for r in resultados),
            'avg_object_percentage': sum(r['object_percentage'] for r in resultados) / len(resultados),
            'avg_background_percentage': sum(r['background_percentage'] for r in resultados) / len(resultados),
            'distance_range': {
                'min': min(r['min_distance'] for r in resultados),
                'max': max(r['max_distance'] for r in resultados)
            }
        }
    }
    
    # Guardar reporte
    with open('analisis_adquisicion_lidar.json', 'w', encoding='utf-8') as f:
        json.dump(reporte, f, indent=2, ensure_ascii=False)
    
    # Mostrar resumen
    print("\n" + "=" * 60)
    print("📈 RESUMEN DEL ANÁLISIS")
    print("=" * 60)
    print(f"• Total de mediciones: {len(resultados)}")
    print(f"• Total de puntos adquiridos: {reporte['summary']['total_points']:,}")
    print(f"• Promedio de puntos objeto: {reporte['summary']['avg_object_percentage']:.1f}%")
    print(f"• Promedio de puntos fondo: {reporte['summary']['avg_background_percentage']:.1f}%")
    print(f"• Rango de distancias: {reporte['summary']['distance_range']['min']:.1f} - {reporte['summary']['distance_range']['max']:.1f} cm")
    
    print(f"\n📊 Archivos generados:")
    print(f"• Adquisicion_Lidar_Medicion_X.png - Visualizaciones por medición")
    print(f"• Analisis_Medicion_X.txt - Texto del análisis por medición")
    print(f"• analisis_adquisicion_lidar.json - Reporte completo")
    
    print(f"\n🎯 El análisis se enfoca en:")
    print(f"• Correspondencia entre nube de puntos y setup experimental")
    print(f"• Vista frontal directamente comparable con fotos")
    print(f"• Separación simple objeto/fondo usando umbral de {BACKGROUND_THRESHOLD}cm")
    print(f"• Filtrado de datos anómalos (distancias > {MAX_DISTANCE_THRESHOLD}cm)")
    print(f"• Distribución de distancias para entender la adquisición")

if __name__ == "__main__":
    analisis_completo_simple()
