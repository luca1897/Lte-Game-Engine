/*
 * LTE Game Engine
 * Copyright (C) 2006-2008 SiberianSTAR <haxormail@gmail.com>
 * http://www.ltestudios.com
 *  
 * The LTE Game Engine is based on Irrlicht 1.0
 * Irrlicht Engine is Copyright (C) 2002-2006 Nikolaus Gebhardt
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */
 
#define NBONES          8
#define MAX_ATTRIB_STACK        16
#define VARRAY_MAX      5       /* number of arrays */
#define MAX_ATTRIB      VARRAY_MAX

struct pspgl_matrix {
	/* Align matrix so VFPU can use aligned memory operations.
	   XXX separate flags so that we don't waste a pile of space
	   in padding. */
	GLfloat mat[16] __attribute__((aligned(VFPU_ALIGNMENT)));
	unsigned flags;
};
struct pspgl_matrix_stack {
	/* adjustments to be applied before use */
	f32 scale[4] __attribute__((aligned(VFPU_ALIGNMENT)));
	f32 trans[4] __attribute__((aligned(VFPU_ALIGNMENT)));

	struct pspgl_matrix *stack;

	unsigned limit;
	unsigned depth;
	unsigned flags;
};
struct pspgl_buffer {
	short refcount;
	short generation;       /* generation counter to detect changes */
	signed char mapped;     /* mapping counter */
	unsigned char flags;
#define BF_PINNED_RD    (1<<0)  /* buffer is pinned for reading (by hardware) */
#define BF_PINNED_WR    (1<<1)  /* buffer if pinned for writing (by hardware) */
#define BF_PINNED (BF_PINNED_RD|BF_PINNED_WR)
#define BF_PINNED_FIXED (1<<2)  /* buffer is pinned permanently */
#define BF_UNMANAGED    (1<<3)  /* buffer is not allocated by us */
#define BF_TRANSIENT    (1<<4)  /* buffer is in the transient pool */
#define BF_MIGRATING    (1<<5)  /* buffer is marked for migration (transient) */

	/* Pointers for the pin list */
	struct pspgl_buffer **pin_prevp;
	struct pspgl_buffer  *pin_next;

	/* Pointer used for the global buffer list */
	struct pspgl_buffer *list_prev;
	struct pspgl_buffer *list_next;

	void *base;
	GLsizeiptr size;
};
struct pspgl_bufferobj {
	int refcount;

	GLenum usage, access;
	GLboolean mapped;       /* glMapBuffer called */

	struct pspgl_buffer *data;
};
struct pspgl_vertex_array {
	GLenum enabled;
	GLint size;
	GLenum type;
	GLsizei stride;
	GLboolean native;       /* size and type match hardware */
	const GLvoid *ptr;

	struct pspgl_bufferobj *buffer;
};
struct t2f_c4ub_n3f_v3f {
	GLfloat texcoord [2];
	unsigned long color;
	GLfloat normal [3];
	GLfloat vertex [3];
};
struct vertex_format
{
	unsigned hwformat;
	unsigned vertex_size;

	unsigned arrays;                /* bitmask of arrays used by this format */

	int nattrib;
	struct attrib {
		unsigned offset;        /* offset into output vertex */
		unsigned size;          /* size of element in output vertex */

		struct pspgl_vertex_array *array; /* source array */

		void (*convert)(void *to, const void *from, const struct attrib *attr);
	} attribs[MAX_ATTRIB];
};
struct pspgl_context {
	struct pspvfpu_context *vfpu_context;

	struct hwstate {
		unsigned dirty;
#define HWD_CLUT        (1 << 0)

		uint32_t ge_reg [256];
		uint32_t ge_reg_touched [256/32];
	} hw;

	struct {
		GLenum primitive;
		unsigned long vertex_count;
		void *vbuf_adr;
		struct t2f_c4ub_n3f_v3f line_loop_start;
	} beginend;
	struct current {
		GLfloat texcoord [4];
		unsigned long color;
		GLfloat normal [3];
	} current;

	struct varray {
		struct pspgl_vertex_array vertex;
		struct pspgl_vertex_array normal;
		struct pspgl_vertex_array color;
		struct pspgl_vertex_array texcoord;
		struct pspgl_vertex_array weight;

		struct locked_arrays {
			GLint first;
			GLsizei count;

			struct vertex_format vfmt;

			struct pspgl_buffer *cached_array;
			unsigned cached_array_offset;
			GLint cached_first;
		} locked;

		struct pspgl_bufferobj *arraybuffer;
		struct pspgl_bufferobj *indexbuffer;
	} vertex_array;

	struct clear {
		uint32_t color;
		GLint stencil;
		unsigned short depth;
	} clear;

	struct scissor_test {
		GLenum enabled;
		GLint x, y, width, height;
	} scissor_test;

	struct write_mask {
		unsigned char alpha;
		unsigned char stencil;
	} write_mask;

	struct lights {
		GLboolean positional[4];        /* set by glLight(N, GL_POSITION, ) */
		GLboolean spotlight[4]; /* set by glLight(N, GL_SPOT_CUTOFF, ) */
	} lights;

	struct material {
		unsigned long ambient;
	} material;

	/* cull_face = (front_cw ^ cull_front) ? ccw : cw */
	struct polygon {
		unsigned char front_cw;
		unsigned char cull_front;
	} polygon;

	struct pspgl_matrix_stack projection_stack;
	struct pspgl_matrix_stack modelview_stack;
	struct pspgl_matrix_stack texture_stack;
	struct pspgl_matrix_stack view_stack;
	struct pspgl_matrix_stack bone_stacks[NBONES];

	struct pspgl_matrix_stack *current_matrix_stack;
	struct pspgl_matrix *current_matrix;


	struct pspgl_shared_context *shared;

	struct pspgl_surface *read;
	struct pspgl_surface *draw;

	struct pspgl_saved_attrib *attribstack[MAX_ATTRIB_STACK];
	unsigned attribstackdepth;

	struct pspgl_saved_clattrib *clattribstack[MAX_ATTRIB_STACK];
	unsigned clattribstackdepth;

	GLenum glerror;
	unsigned int swap_interval;
	int initialized;
	int refcount;

	struct viewport {
		GLint x, y, width, height;
		GLfloat depth_offset;
	} viewport;
	struct fog {
		GLfloat near, far;
	} fog;
	struct texture {
		struct pspgl_texobj     *bound; /* currently bound texture */
	} texture;
	struct pixelstore {
		unsigned row_length;
		unsigned alignment;
		unsigned skip_pixels;
		unsigned skip_rows;
		GLboolean invert;

		struct pspgl_bufferobj *pbo;
	} pack, unpack;
	struct vertexblend {
		GLboolean enabled;
	} vertexblend;
	struct {
		GLboolean enabled;

		/* all times in microseconds */
		unsigned queuewait;      /* time spent waiting for queues */
		unsigned buffer_issues;  /* number of command buffers issued */
	} stats;
};
