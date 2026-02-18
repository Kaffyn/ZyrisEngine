def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return [
        "AbilitySystemComponent",
        "AbilitySystemAttributeSet",
        "AbilitySystemAbility",
        "AbilitySystemAbilitySpec",
        "AbilitySystemEffect",
        "AbilitySystemEffectSpec",
        "AbilitySystemTag",
        "AbilitySystemTagContainer",
        "AbilitySystemCue",
        "AbilitySystemCueSpec",
        "AbilitySystemMagnitudeCalculation",
        "AbilitySystemTargetData",
        "AbilitySystemTask",
        "AbilitySystem",
        "AbilitySystemEditorPlugin",
    ]


def get_icons_path():
    return "icons"
