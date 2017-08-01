import React, { Component } from 'react'
import PropTypes from 'prop-types'

export default class Posts extends Component {
  render() {
    if(this.props.posts) {
      return(
        <ul>
          {this.props.posts.map((post, i) => (
            <li key={i}>
              <a href={post.url}>{post.title}</a>
              <span className="comments"><a href={"https://reddit.com" + post.permalink}>{post.num_comments}</a></span>
              <span className="ups">{post.ups}</span>
            </li>
          ))}
        </ul>
      )
    }
    else {
      return(<p>nothing selected</p>)
    }
  }
}

Posts.propTypes = {
  posts: PropTypes.array.isRequired
}
