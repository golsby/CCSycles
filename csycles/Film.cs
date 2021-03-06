﻿/**
Copyright 2014 Robert McNeel and Associates

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**/

using System.Drawing;

namespace ccl
{
	/// <summary>
	/// Film representation in a Cycles scene.
	/// </summary>
	public class Film
	{
		/// <summary>
		/// Reference to the scene in which this film is contained.
		/// </summary>
		internal Scene Scene { get; set; }

		/// <summary>
		/// Create a new film representation for Scene.
		/// </summary>
		/// <param name="scene"></param>
		internal Film(Scene scene)
		{
			Scene = scene;
		}

		/// <summary>
		/// Set film exposure
		/// </summary>
		public float Exposure
		{
			set
			{
				CSycles.film_set_exposure(Scene.Client.Id, Scene.Id, value);
			}
		}

		/// <summary>
		/// Set film filter type and width
		/// </summary>
		/// <param name="filterType">Box or Gaussian</param>
		/// <param name="filterWidth">for proper Box use 1.0f</param>
		public void SetFilter(FilterType filterType, float filterWidth)
		{
			CSycles.film_set_filter(Scene.Client.Id, Scene.Id, filterType, filterWidth);
		}

		/// <summary>
		/// Tag the film for update.
		/// </summary>
		public void Update()
		{
			CSycles.film_tag_update(Scene.Client.Id, Scene.Id);
		}
	}
}
