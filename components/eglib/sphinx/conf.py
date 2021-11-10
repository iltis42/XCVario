# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import os
import os.path


# -- Project information -----------------------------------------------------

project = "eglib"
copyright = "2020, Fabio Pugliese Ornellas"
author = "Fabio Pugliese Ornellas"

# The full version, including alpha/beta/rc tags
release = "master"


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ["hawkmoth"]

# hawkmoth

if "TOP_SRCDIR" not in os.environ:
    raise RuntimeError("TOP_SRCDIR not set!")
TOP_SRCDIR = os.environ.get("TOP_SRCDIR")
if "TOP_BUILDDIR" not in os.environ:
    raise RuntimeError("TOP_BUILDDIR not set!")
TOP_BUILDDIR = os.environ.get("TOP_BUILDDIR")

cautodoc_root = TOP_BUILDDIR
cautodoc_clang = f"-I{TOP_SRCDIR},-I{TOP_BUILDDIR}"

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "alabaster"
html_theme_options = {
    "logo": "logo.png",
    "github_user": "fornellas",
    "github_repo": "eglib",
    "github_banner": True,
    "github_button": False,
    "github_type": "star",
    "show_relbar_bottom": True,
    "code_font_size": "0.8em",
}
html_sidebars = {
    "**": [
        "about.html",
        "badges.html",
        "donate.html",
        "navigation.html",
        # 'relations.html',
        "searchbox.html",
    ]
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]