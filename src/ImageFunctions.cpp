#include "ImageFunctions.h"

namespace ImageTL
{
	// Generic Unary operations
	std::complex<double> conjugate(std::complex<double> n)
	{
		return conj(n);
	}

	std::complex<double> normalize(std::complex<double> n)
	{
		double inverse_n = abs(n);
		inverse_n = (inverse_n == 0)?0:1./inverse_n;
		return n*inverse_n;
	}

	// Domain transform functions
	void transpose(int &x, int &y)
	{
		int temp = x;
		x = y;
		y = temp;
	}

	// Algorithms
	void GaussianTemplates(ConstantTemplate<double>* &x, ConstantTemplate<double>* &y, double sigma, int gaussWidth)
	{
		if(gaussWidth <= 0) {
			gaussWidth = (int)(sigma*6); }

		if(gaussWidth < 3) {
			gaussWidth = 3; }

		if(gaussWidth%2 == 0) {
			gaussWidth++; }

		int bound = (gaussWidth-1)/2;
		double *gauss = new double[gaussWidth];
		double c = 1./(sqrt(2*PI)*sigma), sum = 0;
		for(int i = -bound; i<=bound; i++)
		{
			gauss[i + bound] = c*exp((-i*i)/(2*sigma*sigma));
			sum += gauss[i + bound];
		}
		ConstantTemplate<double>* gaussian_x = new ConstantTemplate<double>(gauss, gaussWidth, 1);
		ConstantTemplate<double>* gaussian_y = new ConstantTemplate<double>(gauss, 1, gaussWidth);
		delete[] gauss;
		(*gaussian_x) /= sum;
		(*gaussian_x) /= sum;

		x = gaussian_x;
		y = gaussian_y;
	}

	void MarrHildrethEdges(Image<double>& input, double sigma, int debug)
	{
		PgmImage<double> output;

		input.edgeHandling() = edge_clamp;

		if(sigma > 0)
		{
			ConstantTemplate<double> *gaussian_x, *gaussian_y;
			GaussianTemplates(gaussian_x, gaussian_y, sigma);

			input = (input + *gaussian_x) + *gaussian_y;

			delete gaussian_x;
			delete gaussian_y;

			if(debug >= 1)
			{
				output = input;
				output.depthHandling() = lower_translate | upper_scale | upper2_stretch;
				output.write("debug_output\\gauss.pgm", 255);
			}
		}

		ConstantTemplate<double> Laplacian(laplacian, laplacian_w, laplacian_h);
		ConstantTemplate<double> Moore(moore, moore_w, moore_h);

		input = input + Laplacian;

		PgmImage<double> c1 = (input>0);
		PgmImage<double> d1 = (c1 | Moore) & (1. - c1);

		PgmImage<double> c2 = (input<0);
		PgmImage<double> d2 = c2 & ((1. - c2) | Moore);

		if(debug >= 1)
		{
			output = input;
			output.depthHandling() = lower_abs | upper_scale | upper2_stretch;
			output.write("debug_output\\laplacian.pgm", 255);

			output = d1;
			output.depthHandling() = lower_translate | upper_scale | upper2_stretch;
			output.write("debug_output\\d1.pgm", 1);

			output = d2;
			output.depthHandling() = lower_translate | upper_scale | upper2_stretch;
			output.write("debug_output\\d2.pgm", 1);
		}

		input = d1 & d2;
	}

	void CoherenceEnhancingDiffusion(Image<double>& input, int    steps, double stepSize,
														   double sigma, double rho,
														   double alpha, double k,
														   int    debug)
	{
		try
		{
			// This is used to store the debug choice
			char option = 0;

			// *** Make derivative templates
			const double deriv_x[9] = { -3, 0,  3,
									   -10, 0, 10,
									    -3, 0,  3};
			ConstantTemplate<double> dx_t(deriv_x, 3, 3);
			dx_t /= 32.;

			const double deriv_y[9] = {3,  10,  3,
									   0,   0,  0,
									  -3, -10, -3};
			ConstantTemplate<double> dy_t(deriv_y, 3, 3);
			dy_t /= 32.;

			// Calculate the gaussian template for sigma (used for smoothing the input image)
			ConstantTemplate<double> *gaussian_sigma_x, *gaussian_sigma_y;
			GaussianTemplates(gaussian_sigma_x, gaussian_sigma_y, sigma);

			// Calculate the gaussian template for rho (used for smoothing J)
			ConstantTemplate<double> *gaussian_rho_x, *gaussian_rho_y;
			GaussianTemplates(gaussian_rho_x, gaussian_rho_y, rho);

			// Declare images needed for the loop
			Image<double> input_gauss, Ux, Uy;
			Image<double> J_11, J_12, J_22;
			Image<double> b, discrim, eigenvalue1(input, false), eigenvalue2(input, false);
			Image<double> eigenvector1_x, eigenvector1_y, eigenvector2_x, eigenvector2_y, norm;
			Image<double> lambda2(input, false);//, Z;
			Image<double> D_11, D_12, D_22, filter;
			PgmImage<double> output;
			for(int i=0; i<steps; i++)
			{
				if(debug >= 1) {
					std::cout<<std::endl<<i<<std::endl<<"----"; }

				input_gauss = (input + *gaussian_sigma_x) + *gaussian_sigma_y;

				// *** Get the x and y derivatives of the input
				Ux = input_gauss + dx_t;
				Uy = input_gauss + dy_t;

				// *** Calculate J(1,1)
				J_11 = Ux * Ux;
				J_11 = (J_11 + *gaussian_rho_x) + *gaussian_rho_y;

				// *** Calculate J(1,2)
				J_12 = Ux * Uy;
				J_12 = (J_12 + *gaussian_rho_x) + *gaussian_rho_y;

				// *** Calculate J(2,2)
				J_22 = Uy * Uy;
				J_22 = (J_22 + *gaussian_rho_x) + *gaussian_rho_y;

				// Done with the gaussians
				delete gaussian_sigma_x;
				delete gaussian_sigma_y;
				delete gaussian_rho_x;
				delete gaussian_rho_y;

				// *** Calculate the eigenvalues of J, (mu1 >= mu2)
				b = J_11 + J_22;
				discrim = (b*b - 4.*(J_11*J_22 - J_12*J_12)).genericUnary(sqrt);

				eigenvalue1 = (b + discrim)/2.;
				eigenvalue2 = (b - discrim)/2.;

				// *** Calculate v1 (the eigenvector of J for mu1)
				eigenvector1_x = J_12;
				eigenvector1_y = eigenvalue1 - J_11;
				norm = (eigenvector1_x*eigenvector1_x + eigenvector1_y*eigenvector1_y).genericUnary(sqrt) + 1e-100;
				eigenvector1_x /= norm;
				eigenvector1_y /= norm;

				// *** Calculate v2 (the eigenvector of J for mu2)
				eigenvector2_x = J_12;
				eigenvector2_y = eigenvalue2 - J_11;
				norm = (eigenvector2_x*eigenvector2_x + eigenvector2_y*eigenvector2_y).genericUnary(sqrt) + 1e-100;
				eigenvector2_x /= norm;
				eigenvector2_y /= norm;

				// *** Calculate lamba to be used in the diffusion tensor
				// lambda is constructed such that if (mu1 >> mu2)->(lambda2->1 and lambda2 >> lambda1)
				double lambda1 = alpha, diff;
				Image<double>::iterator lambda2_i = lambda2.begin();
				Image<double>::iterator eigen1_i  = eigenvalue1.begin();
				Image<double>::iterator eigen2_i  = eigenvalue2.begin();
				/*Image<double> Z = (double)k/discrim;
				PgmImage<double>::iterator J_12_i    = J_12.begin();
				PgmImage<double>::iterator J_22_i    = J_22.begin();
				PgmImage<double>::iterator J_11_i    = J_11.begin();
				PgmImage<double>::iterator Z_i       = Z.begin();*/
				Image<double>::iterator e = lambda2.end();
				for(; lambda2_i != e; ++lambda2_i, ++eigen1_i, ++eigen2_i)//++Z_i, ++J_12_i, ++J_11_i, ++J_22_i)
				{
					// My version from ppt slides
					if(*eigen1_i == *eigen2_i) {
						*lambda2_i = alpha; }
					else
					{
						diff = *eigen1_i - *eigen2_i;
						*lambda2_i = alpha + (1-alpha)*exp(-k / (diff*diff));
					}

					// Version from matlab files
					/*if(*Z_i > 10) {
						*Z_i = 10.; }

					if(*J_12_i<0.0001 && *J_22_i<0.1 && *J_11_i<0.01)
					{
						*lambda2_i = alpha;
						*Z_i = 5.;
					}
					else {
						*lambda2_i = alpha + (1-alpha)*exp(-(*Z_i)); }*/
				}

				// *** Calculate the diffusion tensor (D)
				D_11 = eigenvector1_x * eigenvector1_x * lambda1 +
					   eigenvector2_x * eigenvector2_x * lambda2;

				D_12 = eigenvector1_x * eigenvector1_y * lambda1 +
					   eigenvector2_x * eigenvector2_y * lambda2;

				D_22 = eigenvector1_y * eigenvector1_y * lambda1 +
					   eigenvector2_y * eigenvector2_y * lambda2;

				// *** Calculate the main diffusion filter image
				Ux = input + dx_t;
				Uy = input + dy_t;

				filter = ((D_11*Ux + D_12*Uy) + dx_t) + ((D_12*Ux + D_22*Uy) + dy_t);

				// *** Calculate the output
				input += stepSize * filter;

				if(debug >= 2)
				{
					output = Ux;
					output.depthHandling() = lower_abs | upper_scale | upper2_stretch;
					output.write("debug_output\\Ux.pgm", 255);

					output = Uy;
					output.depthHandling() = lower_abs | upper_scale | upper2_stretch;
					output.write("debug_output\\Uy.pgm", 255);

					output = lambda2;
					output.depthHandling() = lower_translate | upper_truncate | upper2_stretch;
					output.write("debug_output\\lambda2.pgm", 255);

					output = J_11;
					output.depthHandling() = lower_translate | upper_scale | upper2_stretch;
					output.write("debug_output\\J_11.pgm", 255);

					output = J_12;
					output.depthHandling() = lower_abs | upper_scale | upper2_stretch;
					output.write("debug_output\\J_12.pgm", 255);

					output = J_22;
					output.depthHandling() = lower_translate | upper_scale | upper2_stretch;
					output.write("debug_output\\J_22.pgm", 255);

					output = filter;
					output.depthHandling() = lower_abs | upper_scale | upper2_stretch;
					output.write("debug_output\\filter.pgm", 255);

					output = input;
					output.depthHandling() = lower_truncate | upper_truncate;
					output.write("debug_output\\diffusion.pgm", 255);

					if(option != 'c')
					{
						option = 0;
						while(true)
						{
							std::cout<<std::endl<<"Enter 's' to step, 'x' to exit, or 'c' to continue to the end: ";
							std::cin>>option;
							if(option != 'c' && option != 'x' && option != 's') {
								std::cout<<"Invalid option."<<std::endl<<std::endl; }
							else {
								break; }
						}
						if(option == 'x') {
							break; }
					}
				}
			}
		}
		catch(ImageException &e)
		{
			std::cout<<e.what()<<std::endl;
		}
	}

	/*
	*  draw a line from P0(x0,y0) to P1(x1,y1) onto the image i with intensity value
	*/
	void DrawLine(Image<double> &i, double value, int x0, int y0, int x1, int y1)
	{
		if(x0 < 0 || x0 > i.width() || y0 < 0 || y0 > i.height())
		{
			//intersect p0 with image boundary
		}

		if(x1 < 0 || x1 > i.width() || y1 < 0 || y1 > i.height())
		{
			//intersect p1 with image boundary
		}

		// Make dy>0
		if(y0 > y1)
		{
			// Swao the x's
			int temp = x0;
			x0 = x1;
			x1 = temp;

			// Swap the y's
			temp = y0;
			y0 = y1;
			y1 = temp;
		}

		int dx = x1 - x0;
		int dy = y1 - y0;
		double m = (dx == 0) ? 2. : dy/(double)dx;	// The slope of the line; Avoid a divide by zero error
		int xDir;						// The direction to alter the x-coordinate
		int d;							// The distance between the midpoint and the actual line
		int incr_4;						// The amount to increment on a 4-connected move
		int incr_8;						// The amount to increment on a 8-connected move
		if(m>0. && m<=1.)				// Set the parameters for a line in the first octant
		{
			xDir = 1;
			d = 2*dy - dx;
			incr_4 = 2*dy;
			incr_8 = 2*(dy-dx);
		}
		else if(m>1.)					// Set the parameters for a line in the second octant
		{
			xDir = 1;
			d = dy - 2*dx;
			incr_4 = -2*dx;
			incr_8 = 2*(dy-dx);
			d = -d;
			incr_4 = -incr_4;
			incr_8 = -incr_8;
		}
		else if(m<-1.)					// Set the parameters for a line in the third octant
		{
			xDir = -1;
			d = -dy - 2*dx;
			incr_4 = -2*dx;
			incr_8 = -2*(dy+dx);
		}
		else if(m<0. && m>=-1.)			// Set the parameters for a line in the fourth octant
		{
			xDir = -1;
			d = -2*dy - dx;
			incr_4 = -2*dy;
			incr_8 = -2*(dy+dx);
			d = -d;
			incr_4 = -incr_4;
			incr_8 = -incr_8;
		}

		m = std::fabs(m);
		int x = x0;
		int y = y0;
		bool loop = true;

		i.getPixel(x0, y0) = value;

		while(loop)
		{
			if(d <= 0)
			{
				d += incr_4;
				if(m > 1.) {
					y++; }
				else {
					x += xDir; }
			}
			else
			{
				d += incr_8;
				x += xDir;
				y++;
			}
			loop = (m > 1.) ? (y < y1) : (xDir*x < xDir*x1);
			i.getPixel(x, y) = value;
		}
	}
}	// end namespace