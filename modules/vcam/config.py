def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_classes():
    return [
        "vCamServer",
        "vCam3D",
        "vCam2D",
        "ShotProfile",
        "ShakeProfile",
    ]


def get_doc_path():
    return "doc_classes"
