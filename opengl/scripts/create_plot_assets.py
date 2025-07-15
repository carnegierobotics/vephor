#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

"""
Generate Vephor plot assets.
"""

import argparse
import math
import pathlib

import cv2
import numpy as np
import numpy.typing as npt
from scipy.spatial.transform import Rotation

IMAGE_SIZE = 256

Image = npt.NDArray[np.uint8]


def parse_arguments():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawTextHelpFormatter
    )

    parser.add_argument("assets", type=pathlib.Path, help="Assets output directory.")
    parser.add_argument("-s", "--image-size", type=int, default=IMAGE_SIZE)

    return parser.parse_args()


def blank_image(image_size: int) -> Image:
    return np.zeros((image_size, image_size, 4), dtype=np.uint8)


def circle(
    image_size: int,
    radius: float = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if radius is None:
        radius = image_size // 2

    image = blank_image(image_size)
    for i in range(image.shape[0]):
        for j in range(image.shape[1]):
            point = np.array([i, j])
            distance = np.linalg.norm(
                point - np.array([image_size // 2, image_size // 2])
            )
            if distance < radius:
                image[i, j] = rgba

    return image


def polygon(
    image_size: int,
    vertices: npt.ArrayLike,
    center_in_image: bool = False,
    offset: npt.ArrayLike = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    vertices = np.array(vertices)
    if vertices.ndim != 2 or vertices.shape[1] != 2:
        raise ValueError

    image = blank_image(image_size)

    if offset is None:
        offset = np.zeros((2,))
    offset = np.array(offset)

    if center_in_image:
        min_corner = np.array([np.min(vertices[:, 0]), np.min(vertices[:, 1])])
        max_corner = np.array([np.max(vertices[:, 0]), np.max(vertices[:, 1])])
        content_size = max_corner - min_corner
        margin = (image_size - content_size) // 2
        offset += margin - min_corner

    for i in range(vertices.shape[0]):
        vertices[i, :] = vertices[i, :] + offset

    vertices = vertices.astype(np.int32)
    vertices = vertices.reshape((-1, 1, 2))
    cv2.fillPoly(image, [vertices], rgba)

    return image


def regular_polygon(
    image_size: int,
    num_sides: int,
    radius: float = None,
    rotation_angle: float = 0,
    center_in_image: bool = False,
    offset: npt.ArrayLike = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if radius is None:
        radius = image_size // 2

    center = np.array([image_size // 2, image_size // 2])

    pts = []
    for i in range(num_sides):
        angle = 2 * np.pi * i / num_sides + np.radians(rotation_angle)
        pts.append(
            [center[0] + radius * np.cos(angle), center[1] + radius * np.sin(angle)]
        )
    return polygon(
        image_size, pts, center_in_image=center_in_image, offset=offset, rgba=rgba
    )


def triangle(
    image_size: int,
    height: int = None,
    side_length: int = None,
    radius: float = None,
    rotation_angle: float = 0,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if radius is not None:
        pass
    elif side_length is not None:
        radius = side_length / math.sqrt(3)
    elif height is not None:
        radius = 2 * height / 3
    else:
        raise ValueError

    return regular_polygon(
        image_size,
        num_sides=3,
        radius=radius,
        rotation_angle=rotation_angle,
        center_in_image=True,
        rgba=rgba,
    )


def square(
    image_size: int,
    side_length: int = None,
    radius: float = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if radius is not None:
        pass
    elif side_length is not None:
        radius = side_length / math.sqrt(2)
    else:
        raise ValueError

    return regular_polygon(
        image_size, num_sides=4, radius=radius, rotation_angle=45, rgba=rgba
    )


def rectangle(
    image_size: int,
    width: float = None,
    height: float = None,
    rotation_angle: float = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
):
    center = image_size // 2
    vertices = np.array(
        [
            [width / 2, height / 2],
            [-width / 2, height / 2],
            [-width / 2, -height / 2],
            [width / 2, -height / 2],
        ]
    )

    if rotation_angle is not None:
        rot = Rotation.from_euler("z", rotation_angle, degrees=True)
        for i in range(vertices.shape[0]):
            vertices[i, :] = rot.apply(np.concatenate((vertices[i, :], [0])))[:2]

    for i in range(vertices.shape[0]):
        vertices[i, :] += center

    return polygon(image_size, vertices=vertices, rgba=rgba)


def diamond(
    image_size: int,
    side_length: int = None,
    radius: int = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if radius is not None:
        pass
    elif side_length is not None:
        radius = side_length / math.sqrt(2)
    else:
        raise ValueError
    return regular_polygon(
        image_size, num_sides=4, radius=radius, rotation_angle=0, rgba=rgba
    )


def pentagon(
    image_size: int,
    radius: float,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    return regular_polygon(
        image_size, num_sides=5, radius=radius, rotation_angle=270, rgba=rgba
    )


def hexagon(
    image_size: int,
    radius: float,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    return regular_polygon(
        image_size, num_sides=6, radius=radius, rotation_angle=0, rgba=rgba
    )


def octagon(
    image_size: int,
    radius: float,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    return regular_polygon(
        image_size, num_sides=8, radius=radius, rotation_angle=0, rgba=rgba
    )


def star(
    image_size: int,
    radius: float = None,
    num_points: int = 5,
    rotation_angle: float = 0,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if radius is None:
        radius = image_size // 2

    center = np.array([image_size // 2, image_size // 2])
    points = []
    for i in range(num_points):
        angle_outer = 2 * np.pi * i / num_points + math.radians(rotation_angle)
        angle_inner = 2 * np.pi * (i + 0.5) / num_points + math.radians(rotation_angle)
        points.append(
            (
                center[0] + radius * np.cos(angle_outer),
                center[1] + radius * np.sin(angle_outer),
            )
        )
        points.append(
            (
                center[0] + radius // 2 * np.cos(angle_inner),
                center[1] + radius // 2 * np.sin(angle_inner),
            )
        )

    return polygon(image_size, vertices=points, center_in_image=True, rgba=rgba)


def line(image_size: int, thickness: float, rotation_angle: float) -> Image:
    return rectangle(IMAGE_SIZE, width=IMAGE_SIZE, height=thickness)


def plus_sign(
    image_size: int,
    thickness: float = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if thickness is None:
        thickness = image_size // 2

    image = blank_image(image_size)
    center = image_size // 2

    image[center - thickness // 2 : center + thickness // 2, :] = rgba
    image[:, center - thickness // 2 : center + thickness // 2] = rgba

    return image


def x_sign(
    image_size: int,
    thickness: float = None,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if thickness is None:
        thickness = image_size // 2

    image = blank_image(image_size)
    for i in range(image_size):
        for j in range(image_size):
            if abs(i - j) < thickness // 2 or abs(i + j - image_size) < thickness // 2:
                image[i, j] = rgba

    return image


def arrow(
    image_size: int,
    body_length: float = None,
    head_length: float = None,
    body_thickness: float = None,
    head_thickness: float = None,
    rotation_angle: float = 0,
    rgba: tuple[int, int, int, int] = (255, 255, 255, 255),
) -> Image:
    if body_length is None:
        body_length = 3 * image_size // 4
    if head_length is None:
        head_length = image_size // 4
    if body_thickness is None:
        body_thickness = image_size // 8
    if head_thickness is None:
        head_thickness = head_length

    vertices = np.array(
        [
            [0, body_thickness // 2],
            [body_length, body_thickness // 2],
            [body_length, head_thickness // 2],
            [body_length + head_length, 0],
            [body_length, -head_thickness // 2],
            [body_length, -body_thickness // 2],
            [0, -body_thickness // 2],
        ]
    )

    rot = Rotation.from_euler("z", rotation_angle, degrees=True)
    for i in range(len(vertices)):
        vertices[i, :] = rot.apply(np.concatenate((vertices[i, :], [0])))[:2]

    return polygon(image_size, vertices=vertices, center_in_image=True, rgba=rgba)


if __name__ == "__main__":
    args = parse_arguments()
    assets_path = args.assets
    image_size = args.image_size

    full_radius = image_size // 2
    half_radius = image_size // 4
    thick_line = image_size // 2
    thin_line = image_size // 8

    cv2.imwrite(str(assets_path / "point.png"), circle(image_size, radius=half_radius))
    cv2.imwrite(str(assets_path / "circle.png"), circle(image_size, radius=full_radius))

    cv2.imwrite(
        str(assets_path / "triangle_right.png"),
        triangle(image_size, side_length=image_size, rotation_angle=0),
    )
    cv2.imwrite(
        str(assets_path / "triangle_down.png"),
        triangle(image_size, side_length=image_size, rotation_angle=90),
    )
    cv2.imwrite(
        str(assets_path / "triangle_left.png"),
        triangle(image_size, side_length=image_size, rotation_angle=180),
    )
    cv2.imwrite(
        str(assets_path / "triangle_up.png"),
        triangle(image_size, side_length=image_size, rotation_angle=270),
    )

    cv2.imwrite(str(assets_path / "square.png"), square(image_size, radius=full_radius))
    cv2.imwrite(
        str(assets_path / "diamond.png"), diamond(image_size, radius=full_radius)
    )
    cv2.imwrite(
        str(assets_path / "pentagon.png"), pentagon(image_size, radius=full_radius)
    )
    cv2.imwrite(
        str(assets_path / "hexagon.png"), hexagon(image_size, radius=full_radius)
    )
    cv2.imwrite(
        str(assets_path / "octagon.png"), octagon(image_size, radius=full_radius)
    )

    cv2.imwrite(
        str(assets_path / "plus_thin.png"), plus_sign(image_size, thickness=thin_line)
    )
    cv2.imwrite(str(assets_path / "cross_thin.png"), x_sign(image_size, thickness=thin_line))
    cv2.imwrite(
        str(assets_path / "plus.png"),
        plus_sign(image_size, thickness=thick_line),
    )
    cv2.imwrite(
        str(assets_path / "cross.png"), x_sign(image_size, thickness=thick_line)
    )
    cv2.imwrite(
        str(assets_path / "star.png"),
        star(image_size, radius=image_size // 2, num_points=5, rotation_angle=-90),
    )

    cv2.imwrite(
        str(assets_path / "line_horizontal.png"),
        rectangle(image_size, width=image_size, height=thin_line),
    )
    cv2.imwrite(
        str(assets_path / "line_vertical.png"),
        rectangle(image_size, width=image_size, height=thin_line, rotation_angle=90),
    )
    cv2.imwrite(
        str(assets_path / "slash_forward.png"),
        rectangle(image_size, width=image_size, height=thin_line, rotation_angle=-60),
    )
    cv2.imwrite(
        str(assets_path / "slash_backward.png"),
        rectangle(image_size, width=image_size, height=thin_line, rotation_angle=60),
    )

    cv2.imwrite(
        str(assets_path / "arrow_right.png"),
        arrow(image_size, body_thickness=thin_line, head_thickness=thick_line),
    )
    cv2.imwrite(
        str(assets_path / "arrow_down.png"),
        arrow(
            image_size,
            body_thickness=thin_line,
            head_thickness=thick_line,
            rotation_angle=90,
        ),
    )
    cv2.imwrite(
        str(assets_path / "arrow_left.png"),
        arrow(
            image_size,
            body_thickness=thin_line,
            head_thickness=thick_line,
            rotation_angle=180,
        ),
    )
    cv2.imwrite(
        str(assets_path / "arrow_up.png"),
        arrow(
            image_size,
            body_thickness=thin_line,
            head_thickness=thick_line,
            rotation_angle=-90,
        ),
    )
