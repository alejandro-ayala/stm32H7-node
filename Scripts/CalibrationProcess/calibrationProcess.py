import cv2
import numpy as np
import glob
import subprocess
import os
import sys
import shutil

# -------------------- CONFIGURACIoN --------------------

CHECKERBOARD = (10, 7)  # esquinas interiores por fila y columna
SQUARE_SIZE = 1.5      # tamaño real de una casilla
discarded_images=0
valid_images=0
total_images=0

images=0
# Ruta del directorio con imagenes JPEG
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
        print("[ERROR] Fallo el preprocesamiento:")
        print(result.stderr)
        sys.exit(1)
    print("[INFO] Preprocesamiento finalizado correctamente.")

def load_images_and_find_corners():
    global valid_images
    global total_images

    objp = np.zeros((CHECKERBOARD[1]*CHECKERBOARD[0], 3), np.float32)
    objp[:, :2] = np.mgrid[0:CHECKERBOARD[0], 0:CHECKERBOARD[1]].T.reshape(-1, 2)
    objp *= SQUARE_SIZE
    objpoints = []  # 3D en el mundo real
    imgpoints = []  # 2D en la imagen

    images = glob.glob(IMG_PATTERN)
    if not images:
        print("[ERROR] No se encontraron imagenes en", IMG_DIR)
        sys.exit(1)
    total_images = len(images)
    print(f"[INFO] {total_images} imagenes encontradas.")

    for fname in images:
        global discarded_images
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
            discarded_images+=1
            print(f"[WARNING] No se detectaron esquinas en: {fname}")

    return objpoints, imgpoints, gray.shape[::-1]

def calibrate_camera(objpoints, imgpoints, image_size):
    print("[INFO] Calibrando camara...")
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, image_size, None, None)

    mean_error = 0
    for i in range(len(objpoints)):
        imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[i], tvecs[i], mtx, dist)
        error = cv2.norm(imgpoints[i], imgpoints2, cv2.NORM_L2)/len(imgpoints2)
        mean_error += error
    return mean_error, mtx, dist, rvecs, tvecs

def clean_resources():
    print("[INFO] Borrando el contenido de la carpeta 'resources/'...")
    files = glob.glob(os.path.join(IMG_DIR, '*'))
    #for f in files:
    #    os.remove(f)
    print("[INFO] Carpeta limpiada.")

def save_calibration(filename, camera_matrix, dist_coeffs, rvecs, tvecs, reprojection_error):
    with open(filename, 'w') as f:
        f.write(f"# Error medio de reproyeccion: {reprojection_error:.6f} px\n\n")

        f.write("# Matriz de camara (intrínseca):\n")
        f.write(np.array2string(camera_matrix, separator=', ') + "\n\n")

        f.write("# Coeficientes de distorsion:\n")
        f.write(np.array2string(dist_coeffs, separator=', ') + "\n\n")

        f.write("# Vectores de rotacion:\n")
        for idx, rvec in enumerate(rvecs):
            f.write(f"  Imagen {idx+1}: {rvec.ravel()}\n")
        
        f.write("\n# Vectores de traslacion:\n")
        for idx, tvec in enumerate(tvecs):
            f.write(f"  Imagen {idx+1}: {tvec.ravel()}\n")

    base_dir = 'sucessfullCalibrations'  # nota: "successful" lleva doble "c", pero dejo tu versión
    os.makedirs(base_dir, exist_ok=True)

    # Buscar el número de calibración más alto existente
    existing = [d for d in os.listdir(base_dir) if os.path.isdir(os.path.join(base_dir, d)) and d.startswith("Calibracion")]
    indices = []
    for d in existing:
        try:
            index = int(d.replace("Calibracion", ""))
            indices.append(index)
        except ValueError:
            continue
    next_index = max(indices, default=0) + 1
    new_dir = os.path.join(base_dir, f"Calibracion{next_index}")
    os.makedirs(new_dir, exist_ok=True)

    # Copiar calibrationParams.txt
    if os.path.exists("calibrationParams.txt"):
        shutil.copy("calibrationParams.txt", new_dir)

    # Copiar imágenes JPEG de resources
    jpeg_images = glob.glob("resources/*.jpg") + glob.glob("resources/*.jpeg")
    for image in jpeg_images:
        shutil.copy(image, new_dir)

    print(f"Datos guardados en: {new_dir}")
   
def main():
    run_preprocessing()
    objpoints, imgpoints, image_size = load_images_and_find_corners()
    if len(objpoints) < 5:
        print("[ERROR] Se necesitan al menos 5 imagenes validas para calibrar.")
        clean_resources()
        sys.exit(1)
    valid_images = total_images - discarded_images
    print(f"[INFO] Calibracion realizada con {valid_images} imagenes")
    mean_error, mtx, dist, rvecs, tvecs = calibrate_camera(objpoints, imgpoints, image_size)

    total_error=mean_error/len(objpoints)
    if total_error < ERROR_THRESHOLD:
        print("[INFO] Calibracion exitosa. total error: {}".format(total_error))
        save_calibration("calibrationParams.txt", mtx, dist, rvecs, tvecs, total_error)
    else:
        print(f"[WARNING] Error demasiado alto ({total_error:.4f}px). Calibracion no valida.")
        clean_resources()
        sys.exit(1)


if __name__ == "__main__":
    main()
