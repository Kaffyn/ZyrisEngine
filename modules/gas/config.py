def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_path():
    return "doc_classes"


def get_doc_classes():
    return [
        "AbilitySystemComponent",
        "AttributeSet",
        "GameplayAbility",
        "GameplayAbilitySpec",
        "GameplayEffect",
        "GameplayEffectSpec",
        "GameplayTag",
        "GameplayTagContainer",
        "GameplayCue",
        "GameplayCueSpec",
        "GameplayEditorPlugin",
    ]
