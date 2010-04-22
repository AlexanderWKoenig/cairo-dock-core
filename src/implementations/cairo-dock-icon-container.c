/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#ifdef HAVE_GLITZ
#include <gdk/gdkx.h>
#include <glitz-glx.h>
#include <cairo-glitz.h>
#endif

#include <gtk/gtkgl.h>

#include "../config.h"
#include "cairo-dock-draw.h"
#include "cairo-dock-icons.h"
#include "cairo-dock-surface-factory.h"
#include "cairo-dock-launcher-manager.h"
#include "cairo-dock-separator-manager.h"
#include "cairo-dock-applet-manager.h"
#include "cairo-dock-dock-factory.h"
#include "cairo-dock-modules.h"
#include "cairo-dock-log.h"
#include "cairo-dock-dock-manager.h"
#include "cairo-dock-class-manager.h"
#include "cairo-dock-X-utilities.h"
#include "cairo-dock-draw-opengl.h"
#include "cairo-dock-internal-taskbar.h"
#include "cairo-dock-internal-indicators.h"
#include "cairo-dock-internal-labels.h"
#include "cairo-dock-internal-icons.h"
#include "cairo-dock-container.h"
#include "cairo-dock-desklet-factory.h"
#include "cairo-dock-dialog-manager.h"
#include "cairo-dock-data-renderer.h"
#include "cairo-dock-flying-container.h"
#include "cairo-dock-emblem.h"
#include "cairo-dock-X-manager.h"
#include "cairo-dock-applications-manager.h"
#include "cairo-dock-load.h"
#include "cairo-dock-backends-manager.h"
#include "cairo-dock-icon-loader.h"

#define CAIRO_DOCK_DEFAULT_APPLI_ICON_NAME "default-icon-appli.svg"

extern CairoDock *g_pMainDock;
extern CairoDockDesktopGeometry g_desktopGeometry;

extern gchar *g_cCurrentThemePath;

extern CairoDockImageBuffer g_pIconBackgroundBuffer;
extern CairoDockImageBuffer g_pBoxAboveBuffer;
extern CairoDockImageBuffer g_pBoxBelowBuffer;

extern gboolean g_bUseOpenGL;


static void _cairo_dock_draw_subdock_content_as_emblem (Icon *pIcon, int w, int h, cairo_t *pCairoContext)
{
	//\______________ On dessine les 4 premieres icones du sous-dock en embleme.
	CairoEmblem e;
	memset (&e, 0, sizeof (CairoEmblem));
	e.fScale = .5;
	int i;
	Icon *icon;
	GList *ic;
	for (ic = pIcon->pSubDock->icons, i = 0; ic != NULL && i < 4; ic = ic->next, i++)
	{
		icon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (icon))
		{
			i --;
			continue;
		}
		e.iPosition = i;
		e.pSurface = icon->pIconBuffer;
		cairo_dock_get_icon_extent (icon, CAIRO_CONTAINER (pIcon->pSubDock), &e.iWidth, &e.iHeight);
		
		cairo_save (pCairoContext);
		_cairo_dock_apply_emblem_surface (&e, w, h, pCairoContext);
		cairo_restore (pCairoContext);
	}
}

static void _cairo_dock_draw_subdock_content_as_emblem_opengl (Icon *pIcon, int w, int h)
{
	//\______________ On dessine les 4 premieres icones du sous-dock en embleme.
	CairoEmblem e;
	memset (&e, 0, sizeof (CairoEmblem));
	e.fScale = .5;
	int i;
	Icon *icon;
	GList *ic;
	for (ic = pIcon->pSubDock->icons, i = 0; ic != NULL && i < 4; ic = ic->next, i++)
	{
		icon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (icon))
		{
			i --;
			continue;
		}
		e.iPosition = i;
		e.iTexture = icon->iIconTexture;
		_cairo_dock_apply_emblem_texture (&e, w, h);
	}
}

static void _cairo_dock_draw_subdock_content_as_stack (Icon *pIcon, int w, int h, cairo_t *pCairoContext)
{
	//\______________ On dessine les 4 premieres icones du sous-dock en pile.
	CairoEmblem e;
	memset (&e, 0, sizeof (CairoEmblem));
	e.fScale = .8;
	int i;
	Icon *icon;
	GList *ic;
	for (ic = pIcon->pSubDock->icons, i = 0; ic != NULL && i < 3; ic = ic->next, i++)
	{
		icon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (icon))
		{
			i --;
			continue;
		}
		switch (i)
		{
			case 0:
				e.iPosition = CAIRO_DOCK_EMBLEM_UPPER_RIGHT;
			break;
			case 1:
				if (ic->next == NULL)
					e.iPosition = CAIRO_DOCK_EMBLEM_LOWER_LEFT;
				else
					e.iPosition = CAIRO_DOCK_EMBLEM_MIDDLE;
			break;
			case 2:
				e.iPosition = CAIRO_DOCK_EMBLEM_LOWER_LEFT;
			break;
			default : break;
		}
		
		e.pSurface = icon->pIconBuffer;
		cairo_dock_get_icon_extent (icon, CAIRO_CONTAINER (pIcon->pSubDock), &e.iWidth, &e.iHeight);
		
		cairo_save (pCairoContext);
		_cairo_dock_apply_emblem_surface (&e, w, h, pCairoContext);
		cairo_restore (pCairoContext);
	}
}

static void _cairo_dock_draw_subdock_content_as_stack_opengl (Icon *pIcon, int w, int h)
{
	//\______________ On dessine les 4 premieres icones du sous-dock en pile.
	CairoEmblem e;
	memset (&e, 0, sizeof (CairoEmblem));
	e.fScale = .8;
	int i;
	Icon *icon;
	GList *ic;
	for (ic = pIcon->pSubDock->icons, i = 0; ic != NULL && i < 3; ic = ic->next, i++)
	{
		icon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (icon))
		{
			i --;
			continue;
		}
		switch (i)
		{
			case 0:
				e.iPosition = CAIRO_DOCK_EMBLEM_UPPER_RIGHT;
			break;
			case 1:
				if (ic->next == NULL)
					e.iPosition = CAIRO_DOCK_EMBLEM_LOWER_LEFT;
				else
					e.iPosition = CAIRO_DOCK_EMBLEM_MIDDLE;
			break;
			case 2:
				e.iPosition = CAIRO_DOCK_EMBLEM_LOWER_LEFT;
			break;
			default : break;
		}
		e.iTexture = icon->iIconTexture;
		_cairo_dock_apply_emblem_texture (&e, w, h);
	}
}


static void _cairo_dock_load_box_surface (void)
{
	double fMaxScale = cairo_dock_get_max_scale (g_pMainDock);
	
	cairo_dock_unload_image_buffer (&g_pBoxAboveBuffer);
	cairo_dock_unload_image_buffer (&g_pBoxBelowBuffer);
	
	int iSize = myIcons.tIconAuthorizedWidth[CAIRO_DOCK_LAUNCHER];
	if (iSize == 0)
		iSize = 48;
	iSize *= fMaxScale;
	
	gchar *cUserPath = cairo_dock_generate_file_path ("box-front.png");
	if (! g_file_test (cUserPath, G_FILE_TEST_EXISTS))
	{
		g_free (cUserPath);
		cUserPath = NULL;
	}
	cairo_dock_load_image_buffer (&g_pBoxAboveBuffer,
		cUserPath ? cUserPath : CAIRO_DOCK_SHARE_DATA_DIR"/box-front.png",
		iSize,
		iSize,
		CAIRO_DOCK_FILL_SPACE);
	
	cUserPath = cairo_dock_generate_file_path ("box-back.png");
	if (! g_file_test (cUserPath, G_FILE_TEST_EXISTS))
	{
		g_free (cUserPath);
		cUserPath = NULL;
	}
	cairo_dock_load_image_buffer (&g_pBoxBelowBuffer,
		CAIRO_DOCK_SHARE_DATA_DIR"/box-back.png",
		iSize,
		iSize,
		CAIRO_DOCK_FILL_SPACE);
}

static void _cairo_dock_unload_box_surface (void)
{
	cairo_dock_unload_image_buffer (&g_pBoxAboveBuffer);
	cairo_dock_unload_image_buffer (&g_pBoxBelowBuffer);
}

static void _cairo_dock_draw_subdock_content_as_box (Icon *pIcon, int w, int h, cairo_t *pCairoContext)
{
	cairo_set_operator (pCairoContext, CAIRO_OPERATOR_OVER);
	cairo_save (pCairoContext);
	cairo_scale(pCairoContext,
		(double) w / g_pBoxBelowBuffer.iWidth,
		(double) h / g_pBoxBelowBuffer.iHeight);
	cairo_set_source_surface (pCairoContext,
		g_pBoxBelowBuffer.pSurface,
		0.,
		0.);
	cairo_paint (pCairoContext);
	cairo_restore (pCairoContext);
	
	cairo_save (pCairoContext);
	cairo_scale(pCairoContext,
		.8,
		.8);
	int i;
	Icon *icon;
	GList *ic;
	for (ic = pIcon->pSubDock->icons, i = 0; ic != NULL && i < 3; ic = ic->next, i++)
	{
		icon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (icon))
		{
			i --;
			continue;
		}
		
		cairo_set_source_surface (pCairoContext,
			icon->pIconBuffer,
			.1*w,
			.1*i*h);
		cairo_paint (pCairoContext);
	}
	cairo_restore (pCairoContext);
	
	cairo_scale(pCairoContext,
		(double) w / g_pBoxAboveBuffer.iWidth,
		(double) h / g_pBoxAboveBuffer.iHeight);
	cairo_set_source_surface (pCairoContext,
		g_pBoxAboveBuffer.pSurface,
		0.,
		0.);
	cairo_paint (pCairoContext);
}

static void _cairo_dock_draw_subdock_content_as_box_opengl (Icon *pIcon, int w, int h)
{
	_cairo_dock_set_blend_source ();
	_cairo_dock_apply_texture_at_size (g_pBoxBelowBuffer.iTexture, w, h);
	
	_cairo_dock_set_blend_alpha ();
	int i;
	Icon *icon;
	GList *ic;
	for (ic = pIcon->pSubDock->icons, i = 0; ic != NULL && i < 3; ic = ic->next, i++)
	{
		icon = ic->data;
		if (CAIRO_DOCK_IS_SEPARATOR (icon))
		{
			i --;
			continue;
		}
		glBindTexture (GL_TEXTURE_2D, icon->iIconTexture);
		_cairo_dock_apply_current_texture_at_size_with_offset (.8*w, .8*h, 0., .1*(1-i)*h);
	}
	
	_cairo_dock_apply_texture_at_size (g_pBoxAboveBuffer.iTexture, w, h);
}


void cairo_dock_register_icon_container_renderers (void)
{
	CairoIconContainerRenderer *p;
	p = g_new0 (CairoIconContainerRenderer, 1);
	p->render = _cairo_dock_draw_subdock_content_as_emblem;
	p->render_opengl = _cairo_dock_draw_subdock_content_as_emblem_opengl;
	cairo_dock_register_icon_container_renderer ("Emblem", p);
	
	p = g_new0 (CairoIconContainerRenderer, 1);
	p->render = _cairo_dock_draw_subdock_content_as_stack;
	p->render_opengl = _cairo_dock_draw_subdock_content_as_stack_opengl;
	cairo_dock_register_icon_container_renderer ("Stack", p);
	
	p = g_new0 (CairoIconContainerRenderer, 1);
	p->load = _cairo_dock_load_box_surface;
	p->unload = _cairo_dock_unload_box_surface;
	p->render = _cairo_dock_draw_subdock_content_as_box;
	p->render_opengl = _cairo_dock_draw_subdock_content_as_box_opengl;
	cairo_dock_register_icon_container_renderer ("Box", p);
	
	memset (&g_pBoxAboveBuffer, 0, sizeof (CairoDockImageBuffer));
	memset (&g_pBoxBelowBuffer, 0, sizeof (CairoDockImageBuffer));
}
