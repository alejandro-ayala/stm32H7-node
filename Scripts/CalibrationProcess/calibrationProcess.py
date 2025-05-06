import cv2
import numpy as np
import glob
import subprocess
import os
import sys

# -------------------- CONFIGURACIÓN --------------------

# Dimensiones interiores del tablero (esquinas, no cuadrados)
CHECKERBOARD = (9, 6)

# Ruta del directorio con imágenes JPEG
IMG_DIR = 'resources/'
IMG_PATTERN = os.path.join(IMG_DIR, '*.jpeg')

# Script de preprocesamiento
PREPROCESS_CMD = ['python3.exe', 'PreprocesCameraCalibration.py', IMG_DIR]
ERROR_THRESHOLD = 1.0
# --------------------------------------------------------

def run_preprocessing():
    print("[INFO] Ejecutando script de preprocesamiento...")
    result = subprocess.run(PREPROCESS_CMD, capture_output=True, text=True)
    if result.returncode != 0:
        print("[ERROR] Falló el preprocesamiento:")
        print(result.stderr)
        sys.exit(1)
    print("[INFO] Preprocesamiento finalizado correctamente.")

def load_images_and_find_corners():
    objp = np.zeros((CHECKERBOARD[1]*CHECKERBOARD[0], 3), np.float32)
    objp[:, :2] = np.mgrid[0:CHECKERBOARD[0], 0:CHECKERBOARD[1]].T.reshape(-1, 2)

    objpoints = []  # 3D en el mundo real
    imgpoints = []  # 2D en la imagen

    images = glob.glob(IMG_PATTERN)
    if not images:
        print("[ERROR] No se encontraron imágenes en", IMG_DIR)
        sys.exit(1)

    print(f"[INFO] {len(images)} imágenes encontradas.")

    for fname in images:
        img = cv2.imread(fname)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        ret, corners = cv2.findChessboardCorners(gray, CHECKERBOARD, None)

        if ret:
            objpoints.append(objp)
            refined = cv2.cornerSubPix(gray, corners, (11,11), (-1,-1),
                criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001))
            imgpoints.append(refined)

            # Para verificar visualmente (opcional)
            # img = cv2.drawChessboardCorners(img, CHECKERBOARD, refined, ret)
            # cv2.imshow('Corners', img)
            # cv2.waitKey(100)
        else:
            print(f"[WARNING] No se detectaron esquinas en: {fname}")

    return objpoints, imgpoints, gray.shape[::-1]

def calibrate_camera(objpoints, imgpoints, image_size):
    print("[INFO] Calibrando cámara...")
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, image_size, None, None)
    print(f"[INFO] Error medio de reproyección: {ret:.4f} px")
    return ret, mtx, dist, rvecs, tvecs

def clean_resources():
    print("[INFO] Borrando el contenido de la carpeta 'resources/'...")
    files = glob.glob(os.path.join(IMG_DIR, '*'))
    #for f in files:
    #    os.remove(f)
    print("[INFO] Carpeta limpiada.")

def save_calibration(filename, camera_matrix, dist_coeffs, rvecs, tvecs, reprojection_error):
    with open(filename, 'w') as f:
        f.write(f"# Error medio de reproyección: {reprojection_error:.6f} px\n\n")

        f.write("# Matriz de cámara (intrínseca):\n")
        f.write(np.array2string(camera_matrix, separator=', ') + "\n\n")

        f.write("# Coeficientes de distorsión:\n")
        f.write(np.array2string(dist_coeffs, separator=', ') + "\n\n")

        f.write("# Vectores de rotación:\n")
        for idx, rvec in enumerate(rvecs):
            f.write(f"  Imagen {idx+1}: {rvec.ravel()}\n")
        
        f.write("\n# Vectores de traslación:\n")
        for idx, tvec in enumerate(tvecs):
            f.write(f"  Imagen {idx+1}: {tvec.ravel()}\n")

    
def main():
    run_preprocessing()
    objpoints, imgpoints, image_size = load_images_and_find_corners()
    if len(objpoints) < 5:
        print("[ERROR] Se necesitan al menos 5 imágenes válidas para calibrar.")
        clean_resources()
        sys.exit(1)
        
    ret, mtx, dist, rvecs, tvecs = calibrate_camera(objpoints, imgpoints, image_size)

    if ret < ERROR_THRESHOLD:
        print("[INFO] Calibración exitosa.")
        save_calibration(...)
    else:
        print(f"[WARNING] Error demasiado alto ({ret:.4f}px). Calibración no válida.")
        clean_resources()
        sys.exit(1)

if __name__ == "__main__":
    main()
