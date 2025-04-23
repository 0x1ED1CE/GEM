/*
MIT License

Copyright (c) 2025 Dice

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef GEM_H
#define GEM_H

#define GEM_VERSION_MAJOR 0
#define GEM_VERSION_MINOR 0
#define GEM_VERSION_PATCH 3

#define GEM_TRUE  1
#define GEM_FALSE 0

#define GEM_TEXT_ALIGN_CENTER 0
#define GEM_TEXT_ALIGN_LEFT   1
#define GEM_TEXT_ALIGN_RIGHT  2
#define GEM_TEXT_ALIGN_TOP    3
#define GEM_TEXT_ALIGN_BOTTOM 4

typedef unsigned int gem_uint;
typedef float        gem_real;
typedef char         gem_char;

typedef struct {
	gem_real x, y, z;
} gem_vec3;

typedef struct {
	void    *texture;
	gem_real aw, ah; // Atlas size
	gem_real cw, ch; // Character size
} gem_font;

void gem_video_draw( // Must supply this function
	void    *texture,
	gem_uint color,
	gem_real dx,
	gem_real dy,
	gem_real dw,
	gem_real dh,
	gem_real sx,
	gem_real sy,
	gem_real sw,
	gem_real sh
);

void gem_video_clip( // Must supply this function
	gem_real x,
	gem_real y,
	gem_real w,
	gem_real h
);

gem_vec3 gem_input_poll( // Must supply this function
	gem_uint id
);

void gem_bound_push(
	gem_uint clip,
	gem_real x,
	gem_real y,
	gem_real w,
	gem_real h
);

void gem_bound_drop();

gem_real gem_bound_x();

gem_real gem_bound_y();

gem_real gem_bound_width();

gem_real gem_bound_height();

gem_uint gem_bound_focused();

gem_uint gem_bound_selected();

gem_uint gem_bound_clicked();

gem_vec3 gem_point_state(
	gem_uint id
);

gem_vec3 gem_point_on(
	gem_uint id
);

gem_vec3 gem_point_off(
	gem_uint id
);

void gem_image_draw(
	void    *texture,
	gem_uint color,
	gem_real dx,
	gem_real dy,
	gem_real dw,
	gem_real dh,
	gem_real sx,
	gem_real sy,
	gem_real sw,
	gem_real sh
);

void gem_text_draw(
	gem_char *text,
	gem_font  font,
	gem_uint  color,
	gem_uint  align_v,
	gem_uint  align_h,
	gem_uint  wrap,
	gem_real  scale,
	gem_real  x,
	gem_real  y,
	gem_real  w,
	gem_real  h
);

void gem_begin(
	gem_real screen_width,
	gem_real screen_height
);

#endif

#ifdef GEM_IMPLEMENTATION

#define GEM_MAX_RECTS  256
#define GEM_MAX_CLIPS  256
#define GEM_MAX_POINTS 10

#define GEM_MIN(A,B) ((A)<(B)?(A):(B))
#define GEM_MAX(A,B) ((A)>(B)?(A):(B))

typedef struct {
	gem_uint clip;
	gem_real ax, ay, bx, by;
} gem_rect;

typedef struct {
	gem_real ax, ay, bx, by;
} gem_clip;

typedef struct {
	gem_vec3 current;
	gem_vec3 previous;
	gem_vec3 on;
	gem_vec3 off;
} gem_point;

static gem_rect  gem_rects[GEM_MAX_RECTS];
static gem_clip  gem_clips[GEM_MAX_CLIPS];
static gem_point gem_points[GEM_MAX_POINTS];

static gem_uint gem_rect_id;
static gem_uint gem_clip_id;

static gem_rect gem_current_rect;
static gem_clip gem_current_clip;

void gem_bound_push(
	gem_uint clip,
	gem_real x,
	gem_real y,
	gem_real w,
	gem_real h
) {
	gem_rects[gem_rect_id++] = gem_current_rect;

	gem_current_rect.clip = clip;
	gem_current_rect.ax   = gem_current_rect.ax+x;
	gem_current_rect.ay   = gem_current_rect.ay+y;
	gem_current_rect.bx   = gem_current_rect.ax+w-1;
	gem_current_rect.by   = gem_current_rect.ay+h-1;

	if (clip) {
		gem_clips[gem_clip_id++] = gem_current_clip;

		gem_current_clip.bx = GEM_MIN(
			gem_current_rect.bx,
			gem_current_clip.bx
		);
		gem_current_clip.bx = GEM_MAX(
			gem_current_clip.bx,
			gem_current_clip.ax-1
		);
		gem_current_clip.by = GEM_MIN(
			gem_current_rect.by,
			gem_current_clip.by
		);
		gem_current_clip.by = GEM_MAX(
			gem_current_clip.by,
			gem_current_clip.ay-1
		);
		gem_current_clip.ax = GEM_MIN(
			gem_current_rect.ax,
			gem_current_clip.bx+1
		);
		gem_current_clip.ax = GEM_MAX(
			gem_current_clip.ax,
			gem_current_clip.ax
		);
		gem_current_clip.ay = GEM_MIN(
			gem_current_rect.ay,
			gem_current_clip.by+1
		);
		gem_current_clip.ay = GEM_MAX(
			gem_current_clip.ay,
			gem_current_clip.ay
		);

		gem_video_clip(
			gem_current_clip.ax,
			gem_current_clip.ay,
			gem_current_clip.bx-gem_current_clip.ax+1,
			gem_current_clip.by-gem_current_clip.ay+1
		);
	}
}

void gem_bound_drop() {
	if (gem_current_rect.clip) {
		gem_current_clip = gem_clips[--gem_clip_id];

		gem_video_clip(
			gem_current_clip.ax,
			gem_current_clip.ay,
			gem_current_clip.bx-gem_current_clip.ax+1,
			gem_current_clip.by-gem_current_clip.ay+1
		);
	}

	gem_current_rect = gem_rects[--gem_rect_id];
}

gem_real gem_bound_x() {
	return gem_current_rect.ax;
}

gem_real gem_bound_y() {
	return gem_current_rect.ay;
}

gem_real gem_bound_width() {
	return gem_current_rect.bx-gem_current_rect.ax+1;
}

gem_real gem_bound_height() {
	return gem_current_rect.by-gem_current_rect.ay+1;
}

gem_uint gem_bound_test(
	gem_real x,
	gem_real y
) {
	return (
		x >= gem_current_clip.ax &&
		x <= gem_current_clip.bx &&
		y >= gem_current_clip.ay &&
		y <= gem_current_clip.by &&
		x >= gem_current_rect.ax &&
		x <= gem_current_rect.bx &&
		y >= gem_current_rect.ay &&
		y <= gem_current_rect.by
	);
}

gem_uint gem_bound_focused() {
	for (gem_uint i=0; i<GEM_MAX_POINTS; i++) {
		if (
			gem_points[i].current.z==0 &&
			gem_bound_test(
				gem_points[i].current.x,
				gem_points[i].current.y
			)
		) return i+1;
	}

	return 0;
}

gem_uint gem_bound_selected() {
	for (gem_uint i=0; i<GEM_MAX_POINTS; i++) {
		if (
			gem_points[i].current.z>0 &&
			gem_bound_test(
				gem_points[i].on.x,
				gem_points[i].on.y
			) && gem_bound_test(
				gem_points[i].current.x,
				gem_points[i].current.y
			)
		) return i+1;
	}

	return 0;
}

gem_uint gem_bound_clicked() {
	for (gem_uint i=0; i<GEM_MAX_POINTS; i++) {
		if (
			gem_points[i].current.z<=0 &&
			gem_points[i].previous.z>0 &&
			gem_bound_test(
				gem_points[i].on.x,
				gem_points[i].on.y
			) &&
			gem_bound_test(
				gem_points[i].off.x,
				gem_points[i].off.y
			)
		) return i+1;
	}

	return 0;
}

gem_vec3 gem_point_state(
	gem_uint id
) {
	if (--id>=GEM_MAX_POINTS)
		return (gem_vec3){0,0,-1};

	return gem_points[id].current;
}

gem_vec3 gem_point_on(
	gem_uint id
) {
	if (--id>=GEM_MAX_POINTS)
		return (gem_vec3){0,0,-1};

	return gem_points[id].on;
}

gem_vec3 gem_point_off(
	gem_uint id
) {
	if (--id>=GEM_MAX_POINTS)
		return (gem_vec3){0,0,-1};

	return gem_points[id].off;
}

void gem_image_draw(
	void    *texture,
	gem_uint color,
	gem_real dx,
	gem_real dy,
	gem_real dw,
	gem_real dh,
	gem_real sx,
	gem_real sy,
	gem_real sw,
	gem_real sh
) {
	dx += gem_current_rect.ax;
	dy += gem_current_rect.ay;

	if (
		dx    >  gem_current_clip.bx ||
		dx+dw <= gem_current_clip.ax ||
		dy    >  gem_current_clip.by ||
		dy+dh <= gem_current_clip.ay
	) return;

	gem_video_draw(
		texture,
		color,
		dx, dy, dw, dh,
		sx, sy, sw, sh
	);
}

void gem_text_draw(
	gem_char *text,
	gem_font  font,
	gem_uint  color,
	gem_uint  align_v,
	gem_uint  align_h,
	gem_uint  wrap,
	gem_real  scale,
	gem_real  x,
	gem_real  y,
	gem_real  w,
	gem_real  h
) {
	x += gem_current_rect.ax;
	y += gem_current_rect.ay;

	gem_real cw = font.cw*scale;
	gem_real ch = font.ch*scale;
	gem_uint cc = font.aw/font.cw;
	gem_real lw = 0;
	gem_real lh = ch;

	for (gem_uint i=0; text[i]; i++) {
		if (text[i]=='\n' || (wrap && lw>w-cw)) {
			lh += ch;
			lw  = 0;
		} else {
			lw += cw;
		}
	}

	gem_uint ti = 0;
	gem_uint si;
	gem_real ox;
	gem_real oy;

	switch (align_v) {
		default:
		case GEM_TEXT_ALIGN_CENTER: oy = y+h/2-lh/2; break;
		case GEM_TEXT_ALIGN_TOP:    oy = y;          break;
		case GEM_TEXT_ALIGN_BOTTOM: oy = y+h-lh;     break;
	}

	while (text[ti]) {
		si = ti;
		lw = 0;

		while (text[ti]) {
			if (text[ti]=='\n' || (wrap && lw>w-cw)) break;

			lw += cw;
			ti += 1;
		}

		switch (align_h) {
			default:
			case GEM_TEXT_ALIGN_CENTER: ox = x+w/2-lw/2; break;
			case GEM_TEXT_ALIGN_LEFT:   ox = x;          break;
			case GEM_TEXT_ALIGN_RIGHT:  ox = x+w-lw;     break;
		}

		for (gem_uint i=si; i<ti; i++) {
			gem_uint code = (gem_uint)text[i];
			gem_real sx   = (gem_real)((code-33)%cc)*font.cw;
			gem_real sy   = (gem_real)((code-33)/cc)*font.ch;

			gem_video_draw(
				font.texture,
				color,
				ox,oy,
				cw,ch,
				sx,sy,
				font.cw,font.ch
			);

			ox += cw;
		}

		oy += ch;
	}
}

void gem_begin(
	gem_real screen_width,
	gem_real screen_height
) {
	gem_rect_id = 0;
	gem_clip_id = 0;

	gem_current_rect.clip = GEM_FALSE;
	gem_current_rect.ax   = 0;
	gem_current_rect.ay   = 0;
	gem_current_rect.bx   = screen_width-1;
	gem_current_rect.by   = screen_height-1;

	gem_current_clip.ax = 0;
	gem_current_clip.ay = 0;
	gem_current_clip.bx = screen_width-1;
	gem_current_clip.by = screen_height-1;

	for (gem_uint i=0; i<GEM_MAX_POINTS; i++) {
		gem_points[i].previous = gem_points[i].current;
		gem_points[i].current  = gem_input_poll(i+1);

		if (
			gem_points[i].previous.z==0 &&
			gem_points[i].current.z!=0
		) {
			gem_points[i].on = gem_points[i].current;
		} else if (
			gem_points[i].previous.z!=0 &&
			gem_points[i].current.z==0
		) {
			gem_points[i].off = gem_points[i].current;
		}
	}
}

#endif
